#include "simplay/platform/image.h"

#include <stdio.h>
#include <stdlib.h>

namespace sim {
  void FloatImage::init(u32 new_w, u32 new_h) {
    release();
    w = new_w;
    h = new_h;
    pixels = (Color3*)malloc(w*h * sizeof(Color3));
  }

  void FloatImage::release() {
    free(pixels);
    pixels = nullptr;
    w = 0;
    h = 0;
  }

  namespace {
#pragma pack(push, 1)
    struct IhdrChunkData {
      u32 width;
      u32 height;
      u8 bit_depth;
      u8 color_type;
      u8 compression_method;
      u8 filter_method;
      u8 interlace_method;

      IhdrChunkData()
          : width(0), height(0), bit_depth(0), color_type(0)
          , compression_method(0), filter_method(0), interlace_method(0) {}
      
      void write_to(FILE* output) const;
    };
#pragma pack(pop)

    struct BufferSpan {
      const u8* value;
      usize size;

      BufferSpan() : value(nullptr), size(0) {}
      BufferSpan(const u8* value, usize size) : value(value), size(size) {}
    };

    constexpr u32 encode_chunk_type(const char *data) {
      return (u32)((data[0]<<24) + (data[1]<<16) + (data[2]<<8) + data[3]);
    }

    struct Chunk {
      enum Type : u32 {
        NONE = 0,
        IHDR = encode_chunk_type("IHDR"),
        IEND = encode_chunk_type("IEND"),
        IDAT = encode_chunk_type("IDAT"),
      };

      u32 length;
      u32 checksum;
      Type type;
      union {
        IhdrChunkData ihdr;
        u8* idat_data;
      };

      Chunk() : length(0), type(), checksum(0) {}

      static Chunk make_IHDR(const IhdrChunkData& data) {
        Chunk res;
        res.length = sizeof(IhdrChunkData);
        res.type = IHDR;
        res.ihdr = data;
        return res;
      }

      static Chunk make_IEND() {
        Chunk res;
        res.type = IEND;
        return res;
      }

      static Chunk make_IDAT() {
        Chunk res;
        res.type = IDAT;
        res.idat_data = nullptr;
        return res;
      }

      Chunk& compute_checksum();

      void write_to(FILE* output) const;
      void write_data_to(FILE* output) const;
    };
  }

  void FloatImage::save_png_to(const char* out_path) {
    if (!out_path)
      return;
    
    FILE* out_file = nullptr;
    if (errno_t err = fopen_s(&out_file, out_path, "wb")) {
      fprintf(stderr, "Failed to open file: \"%s\"", out_path);
      return;
    }

    u8 header[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    fwrite(header, sizeof(u8), 8, out_file);

    IhdrChunkData ihdr_data;
    ihdr_data.width = 0;
    ihdr_data.height = 0;
    ihdr_data.bit_depth = 8;
    ihdr_data.color_type = 3; // Truecolor.
    ihdr_data.compression_method = 0;
    ihdr_data.filter_method = 0;
    ihdr_data.interlace_method = 0; // No interlacing.
    Chunk::make_IHDR(ihdr_data).compute_checksum().write_to(out_file);

    Chunk idat = Chunk::make_IDAT();
    idat.compute_checksum().write_to(out_file);
    
    Chunk::make_IEND().compute_checksum().write_to(out_file);    
    fclose(out_file);
  }

  namespace {
    // https://github.com/gcc-mirror/gcc/blob/master/libiberty/crc32.c
    const unsigned int crc32_table[] = {
      0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
      0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
      0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
      0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
      0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
      0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
      0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
      0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
      0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
      0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
      0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
      0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
      0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
      0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
      0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
      0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
      0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
      0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
      0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
      0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
      0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
      0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
      0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
      0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
      0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
      0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
      0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
      0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
      0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
      0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
      0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
      0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
      0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
      0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
      0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
      0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
      0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
      0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
      0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
      0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
      0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
      0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
      0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
      0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
      0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
      0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
      0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
      0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
      0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
      0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
      0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
      0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
      0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
      0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
      0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
      0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
      0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
      0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
      0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
      0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
      0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
      0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
      0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
      0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
    };

    u32 compute_crc32(const u8* buf, usize len, u32 init) {
      u32 crc = init;
      while (len--) {
        crc = (crc<<8) ^ crc32_table[((crc>>24) ^ *buf) & 255];
        ++buf;
      }
      return crc;
    }

    Chunk& Chunk::compute_checksum() {
      checksum = compute_crc32((u8*)&type, (4 + length)*sizeof(u8), 0);
      return *this;
    }

    void write_be(u32 value, FILE* out_file) {
      u8 bits[4] = {};
      bits[3] = (value >> 0) & 0xFF;
      bits[2] = (value >> 8) & 0xFF;
      bits[1] = (value >> 16) & 0xFF;
      bits[0] = (value >> 24) & 0xFF;
      fwrite(bits, sizeof(u8), 4, out_file);
    }

    void Chunk::write_to(FILE* out_file) const {
      write_be(length, out_file);
      write_be(type, out_file);
      write_data_to(out_file);
      write_be(checksum, out_file);
    }

    void Chunk::write_data_to(FILE* out_file) const {
      switch (type) {
        case NONE:
        default:
          break;
        case IHDR:
          ihdr.write_to(out_file);
          break;
        case IDAT:
          fwrite(idat_data, sizeof(u8), length, out_file);
          break;
      }
    }

    void IhdrChunkData::write_to(FILE* out_file) const {
      write_be(width, out_file);
      write_be(height, out_file);
      fwrite(&bit_depth, sizeof(u8), 1, out_file);
      fwrite(&color_type, sizeof(u8), 1, out_file);
      fwrite(&compression_method, sizeof(u8), 1, out_file);
      fwrite(&filter_method, sizeof(u8), 1, out_file);
      fwrite(&interlace_method, sizeof(u8), 1, out_file);
    }
  }
}
