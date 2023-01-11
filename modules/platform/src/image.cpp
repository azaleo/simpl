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
      
      u32 compute_checksum(u32 init) const;
      void write_to(FILE* out) const;
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
      Type type;
      union {
        IhdrChunkData ihdr;
        u8* idat_data;
      };

      Chunk() : length(0), type() {}

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

      u32 compute_checksum() const;
      void write_to(FILE* out) const;
      void write_data_to(FILE* out) const;
    };
  }

  void FloatImage::save_png_to(const char* out_path) {
    if (!out_path)
      return;
    
    FILE* out = nullptr;
    if (errno_t err = fopen_s(&out, out_path, "wb")) {
      fprintf(stderr, "Failed to open file: \"%s\"", out_path);
      return;
    }

    u8 header[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    fwrite(header, sizeof(u8), 8, out);

    IhdrChunkData ihdr_data;
    ihdr_data.width = 1;
    ihdr_data.height = 1;
    ihdr_data.bit_depth = 8;
    ihdr_data.color_type = 0; // Grayscale.
    ihdr_data.compression_method = 0;
    ihdr_data.filter_method = 0;
    ihdr_data.interlace_method = 0; // No interlacing.
    Chunk::make_IHDR(ihdr_data).write_to(out);

    Chunk idat = Chunk::make_IDAT();
    idat.length = 13;
    idat.idat_data = (u8*)malloc(idat.length*sizeof(u8));
    idat.idat_data[0] = 0x08;
    idat.idat_data[1] = 0x1D;
    idat.idat_data[2] = 0x01;
    idat.idat_data[3] = 0x02;
    idat.idat_data[4] = 0x00;
    idat.idat_data[5] = 0xFD;
    idat.idat_data[6] = 0xFF;
    idat.idat_data[7] = 0x00;
    idat.idat_data[8] = 0x00;
    idat.idat_data[9] = 0x00;
    idat.idat_data[10] = 0x02;
    idat.idat_data[11] = 0x00;
    idat.idat_data[12] = 0x01;
    idat.write_to(out);
    free(idat.idat_data);
    
    Chunk::make_IEND().write_to(out);    
    fclose(out);
  }

  namespace {
    template <usize Size>
    struct Buffer {
      u8 bytes[Size];

      Buffer() : bytes{} {}
    };

    Buffer<4> make_be(u32 value) {
      Buffer<4> be = {};
      be.bytes[0] = (value >> 24) & 0xFF;
      be.bytes[1] = (value >> 16) & 0xFF;
      be.bytes[2] = (value >> 8) & 0xFF;
      be.bytes[3] = (value >> 0) & 0xFF;
      return be;
    }

    // https://www.w3.org/TR/png/#D-CRCAppendix
    namespace crc {
      u32 crc32_table[256] = {};
      bool crc32_table_initialized = false;

      void compute_crc32_table() {
        for (u32 i = 0; i < 256; ++i) {
          u32 c = i;
          for (u32 j = 0; j < 8; ++j)
            c = (c & 0x1) ? ((c>>1) ^ 0xEDB88320) : (c>>1);
          crc32_table[i] = c;
        }
        crc32_table_initialized = true;
      }

      u32 compute_crc32(u32 init, const u8* buf, u32 len) {
        if (!crc32_table_initialized)
          compute_crc32_table();
      
        u32 crc = ~init & 0xFFFFFFFF;
        for (u32 i = 0; i < len; ++i)
          crc = (crc>>8) ^ crc32_table[(crc ^ buf[i]) & 0xFF];
        return crc ^ 0xFFFFFFFF;
      }
    }

    u32 Chunk::compute_checksum() const {
      u32 checksum = crc::compute_crc32(0, make_be(type).bytes, 4*sizeof(u8));
      switch (type) {
        case NONE:
        default:
          break;
        case IHDR:
          checksum = ihdr.compute_checksum(checksum);
          break;
      }
      return checksum;
    }

    void write_be(u32 value, FILE* out) {
      fwrite(make_be(value).bytes, sizeof(u8), 4, out);
    }

    void Chunk::write_to(FILE* out) const {
      write_be(length, out);
      write_be(type, out);
      write_data_to(out);
      write_be(compute_checksum(), out);
    }

    void Chunk::write_data_to(FILE* out) const {
      switch (type) {
        case NONE:
        default:
          break;
        case IHDR:
          ihdr.write_to(out);
          break;
        case IDAT:
          fwrite(idat_data, sizeof(u8), length, out);
          break;
      }
    }

    u32 IhdrChunkData::compute_checksum(u32 init) const {
      u32 checksum = init;
      checksum = crc::compute_crc32(checksum, make_be(width).bytes, 4*sizeof(u8));
      checksum = crc::compute_crc32(checksum, make_be(height).bytes, 4*sizeof(u8));
      checksum = crc::compute_crc32(checksum, &bit_depth, 5*sizeof(u8));
      return checksum;
    }

    void IhdrChunkData::write_to(FILE* out) const {
      write_be(width, out);
      write_be(height, out);
      fwrite(&bit_depth, sizeof(u8), 5, out);
    }
  }
}
