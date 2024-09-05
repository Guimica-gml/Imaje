#ifndef IMAJE_H
#define IMAJE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

typedef uint16_t u16;
typedef uint8_t byte;

typedef struct {
    byte *data;
    size_t count;
} Imj_Bytes;

typedef struct {
    const byte *data;
    size_t count;
} Imj_Bytes_View;

typedef enum {
    IMJ_SOI = 0,
    IMJ_APP0,
    IMJ_APP15,
    IMJ_SOF0,
    IMJ_SOF1,
    IMJ_SOF2,
    IMJ_SOF3,
    IMJ_SOF5,
    IMJ_SOF6,
    IMJ_SOF7,
    IMJ_SOF9,
    IMJ_SOF10,
    IMJ_SOF11,
    IMJ_SOF13,
    IMJ_SOF14,
    IMJ_SOF15,
    IMJ_DHT,
    IMJ_DQT,
    IMJ_SOS,
    IMJ_JPG,
    IMJ_JPG0,
    IMJ_JPG13,
    IMJ_DAC,
    IMJ_DNL,
    IMJ_DRI,
    IMJ_DHP,
    IMJ_EXP,
    IMJ_RST0,
    IMJ_RST1,
    IMJ_RST2,
    IMJ_RST3,
    IMJ_RST4,
    IMJ_RST5,
    IMJ_RST6,
    IMJ_RST7,
    IMJ_TEM,
    IMJ_COM,
    IMJ_EOI,
} Imj_Header_Type;

typedef enum {
    PIXEL_ASPECT_RATIO = 0,
    PIXELS_PER_INCH,
    PIXELS_PER_CM,
} Imj_Density_Unit_Type;

typedef struct {
    byte major_version_number;
    byte minor_version_number;

    Imj_Density_Unit_Type density_unit_type;
    u16 x_density;
    u16 y_density;

    byte thumbnail_width;
    byte thumbnail_height;
    Imj_Bytes_View thumbnail_bytes;
} Imj_APP0;

typedef struct {
    byte id;
    Imj_Bytes_View qt_bytes;
} Imj_DQT;

// NOTE(nic): in theory it starts at 1, but I decide stuff here
typedef enum {
    IMJ_COMP_Y = 0,
    IMJ_COMP_CB,
    IMJ_COMP_CR,
    IMJ_COMP_I,
    IMJ_COMP_Q,
    IMJ_COMP_COUNT,
} Imj_Comp_Id;

typedef struct {
    Imj_Comp_Id id;
    byte vertical_sampling_factor;
    byte horizontal_sampling_factor;
    byte qt_number;
} IMj_SOF0_Comp;

typedef struct {
    byte data_precision; // In bits
    u16 image_width;
    u16 image_height;
    IMj_SOF0_Comp components[IMJ_COMP_COUNT];
    byte components_count;
} Imj_SOF0;

typedef enum {
    IMJ_HT_DC,
    IMJ_HT_AC,
} Imj_HT_Type;

typedef struct {
    byte id;
    Imj_HT_Type type;
    Imj_Bytes_View symbols;
} Imj_DHT;

typedef struct {
    Imj_Comp_Id id;
    byte dc_ht_id;
    byte ac_ht_id;
} Imj_SOS_Comp;

typedef struct {
    Imj_Bytes_View compressed_data_bytes;
    Imj_SOS_Comp components[IMJ_COMP_COUNT];
    byte components_count;
} Imj_SOS;

#define DQTS_CAP 4
#define DHTS_CAP 4

typedef struct {
    Imj_APP0 app0;
    Imj_SOF0 sof0;
    Imj_SOS sos;

    Imj_DQT dqts[DQTS_CAP];
    size_t dqts_count;

    Imj_DHT dhts[DHTS_CAP];
    size_t dhts_count;
} Imj_JPG;

const char *imj_header_type_to_cstr(Imj_Header_Type type);
const char *imj_density_unit_type_to_cstr(Imj_Density_Unit_Type type);
const char *imj_comp_id_to_cstr(Imj_Comp_Id id);
const char *imj_ht_type_to_cstr(Imj_HT_Type type);

Imj_Header_Type imj_header_type_from_byte(byte byte);

Imj_Bytes imj_read_file(const char *filepath);
Imj_Bytes_View imj_read_bytes(Imj_Bytes_View *bytes, size_t count);
byte imj_read_byte(Imj_Bytes_View *bytes);
u16 imj_read_be_u16(Imj_Bytes_View *bytes);

Imj_APP0 imj_parse_app0(Imj_Bytes_View bytes);
Imj_DQT imj_parse_dqt(Imj_Bytes_View bytes);
Imj_SOF0 imj_parse_sof0(Imj_Bytes_View bytes);
Imj_DHT imj_parse_dht(Imj_Bytes_View bytes);
Imj_SOS imj_parse_sos(Imj_Bytes_View bytes);

#endif // IMAJE_H

#ifdef IMAJE_IMPLEMTATION
#undef IMAJE_IMPLEMTATION

const char *imj_header_type_to_cstr(Imj_Header_Type type) {
    switch (type) {
    case IMJ_SOI: return "SOI";
    case IMJ_APP0: return "APP0";
    case IMJ_APP15: return "APP15";
    case IMJ_SOF0: return "SOF0";
    case IMJ_SOF1: return "SOF1";
    case IMJ_SOF2: return "SOF2";
    case IMJ_SOF3: return "SOF3";
    case IMJ_SOF5: return "SOF5";
    case IMJ_SOF6: return "SOF6";
    case IMJ_SOF7: return "SOF7";
    case IMJ_SOF9: return "SOF9";
    case IMJ_SOF10: return "SOF10";
    case IMJ_SOF11: return "SOF11";
    case IMJ_SOF13: return "SOF13";
    case IMJ_SOF14: return "SOF14";
    case IMJ_SOF15: return "SOF15";
    case IMJ_DHT: return "DHT";
    case IMJ_DQT: return "DQT";
    case IMJ_SOS: return "SOS";
    case IMJ_JPG: return "JPG";
    case IMJ_JPG0: return "JPG0";
    case IMJ_JPG13: return "JPG13";
    case IMJ_DAC: return "DAC";
    case IMJ_DNL: return "DNL";
    case IMJ_DRI: return "DRI";
    case IMJ_DHP: return "DHP";
    case IMJ_EXP: return "EXP";
    case IMJ_RST0: return "RST0";
    case IMJ_RST1: return "RST1";
    case IMJ_RST2: return "RST2";
    case IMJ_RST3: return "RST3";
    case IMJ_RST4: return "RST4";
    case IMJ_RST5: return "RST5";
    case IMJ_RST6: return "RST6";
    case IMJ_RST7: return "RST7";
    case IMJ_TEM: return "TEM";
    case IMJ_COM: return "COM";
    case IMJ_EOI: return "EOI";
    default: {
        fprintf(stderr, "Error: invalid header type: 0x%02X\n", type);
        exit(1);
    }
    }
}

const char *imj_density_unit_type_to_cstr(Imj_Density_Unit_Type type) {
    switch (type) {
    case PIXEL_ASPECT_RATIO: return "PIXEL_ASPECT_RATIO";
    case PIXELS_PER_INCH: return "PIXELS_PER_INCH";
    case PIXELS_PER_CM: return "PIXELS_PER_CM";
    default: {
        fprintf(stderr, "Error: invalid density unit type: %d\n", type);
        exit(1);
    }
    }
}

const char *imj_comp_id_to_cstr(Imj_Comp_Id id) {
    switch (id) {
    case IMJ_COMP_Y: return "COMP_Y";
    case IMJ_COMP_CB: return "COMP_CB";
    case IMJ_COMP_CR: return "COMP_CR";
    case IMJ_COMP_I: return "COMP_I";
    case IMJ_COMP_Q: return "COMP_Q";
    default: {
        fprintf(stderr, "Error: invalid component id: %d\n", id);
        exit(1);
    }
    }
}

const char *imj_ht_type_to_cstr(Imj_HT_Type type) {
    switch (type) {
    case IMJ_HT_DC: return "HT_DC";
    case IMJ_HT_AC: return "HT_AC";
    default: {
        fprintf(stderr, "Error: invalid huffman table type: %d\n", type);
        exit(1);
    }
    }
}

Imj_Header_Type imj_header_type_from_byte(byte byte) {
    switch (byte) {
    case 0xD8: return IMJ_SOI;
    case 0xE0: return IMJ_APP0;
    case 0xEF: return IMJ_APP15;
    case 0xC0: return IMJ_SOF0;
    case 0xC1: return IMJ_SOF1;
    case 0xC2: return IMJ_SOF2;
    case 0xC3: return IMJ_SOF3;
    case 0xC5: return IMJ_SOF5;
    case 0xC6: return IMJ_SOF6;
    case 0xC7: return IMJ_SOF7;
    case 0xC9: return IMJ_SOF9;
    case 0xCA: return IMJ_SOF10;
    case 0xCB: return IMJ_SOF11;
    case 0xCD: return IMJ_SOF13;
    case 0xCE: return IMJ_SOF14;
    case 0xCF: return IMJ_SOF15;
    case 0xC4: return IMJ_DHT;
    case 0xDB: return IMJ_DQT;
    case 0xDA: return IMJ_SOS;
    case 0xC8: return IMJ_JPG;
    case 0xF0: return IMJ_JPG0;
    case 0xFD: return IMJ_JPG13;
    case 0xCC: return IMJ_DAC;
    case 0xDC: return IMJ_DNL;
    case 0xDD: return IMJ_DRI;
    case 0xDE: return IMJ_DHP;
    case 0xDF: return IMJ_EXP;
    case 0xD0: return IMJ_RST0;
    case 0xD1: return IMJ_RST1;
    case 0xD2: return IMJ_RST2;
    case 0xD3: return IMJ_RST3;
    case 0xD4: return IMJ_RST4;
    case 0xD5: return IMJ_RST5;
    case 0xD6: return IMJ_RST6;
    case 0xD7: return IMJ_RST7;
    case 0x01: return IMJ_TEM;
    case 0xFE: return IMJ_COM;
    case 0xD9: return IMJ_EOI;
    default: {
        fprintf(stderr, "Error: invalid header type: 0x%02X\n", byte);
        exit(1);
    }
    }
}

Imj_Bytes imj_read_file(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not read '%s': %s\n", filepath, strerror(errno));
        exit(1);
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: could not read '%s': %s\n", filepath, strerror(errno));
        exit(1);
    }

    size_t count = ftell(file);
    rewind(file);

    Imj_Bytes bytes = {0};
    bytes.data = malloc(count * sizeof(byte));
    bytes.count = count;

    size_t read_count = fread(bytes.data, sizeof(byte), count, file);
    assert(count == read_count);
    return bytes;
}

Imj_Bytes_View imj_read_bytes(Imj_Bytes_View *bytes, size_t count) {
    if (bytes->count < count) {
        fprintf(stderr, "Error: could not read %zu byte(s)\n", count);
        exit(1);
    }

    Imj_Bytes_View view = {0};
    view.data = bytes->data;
    view.count = count;

    bytes->data += count;
    bytes->count -= count;
    return view;
}

byte imj_read_byte(Imj_Bytes_View *bytes) {
    byte byte = bytes->data[0];
    bytes->data += 1;
    bytes->count -= 1;
    return byte;
}

u16 imj_read_be_u16(Imj_Bytes_View *bytes) {
    if (bytes->count < 2) {
        fprintf(stderr, "Error: could not read 2 byte(s)\n");
        exit(1);
    }

    byte temp[2];
    temp[1] = imj_read_byte(bytes);
    temp[0] = imj_read_byte(bytes);
    return *(u16*)temp;
}

Imj_APP0 imj_parse_app0(Imj_Bytes_View bytes) {
    Imj_APP0 app0 = {0};

    Imj_Bytes_View ident = imj_read_bytes(&bytes, 5);

    byte expected_ident[5] = { 0x4A, 0x46, 0x49, 0x46, 0x00 };
    if (memcmp(ident.data, expected_ident, ident.count)) {
        fprintf(stderr, "Error: invalid APP0 marker segment: incorrect identifier sequence\n");
        exit(1);
    }

    app0.major_version_number = imj_read_byte(&bytes);
    app0.minor_version_number = imj_read_byte(&bytes);

    byte density_unit_type = imj_read_byte(&bytes);
    if (density_unit_type > 2) {
        fprintf(stderr, "Error: invalid APP0 marker segment: incorrect density unit value: %d\n", density_unit_type);
        exit(1);
    }
    app0.density_unit_type = density_unit_type;

    app0.x_density = imj_read_be_u16(&bytes);
    app0.y_density = imj_read_be_u16(&bytes);

    if (app0.x_density == 0 || app0.y_density == 0) {
        fprintf(stderr, "Error: invalid APP0 marker segment: x density and y density cannot be zero\n");
        exit(1);
    }

    app0.thumbnail_width = imj_read_byte(&bytes);
    app0.thumbnail_height = imj_read_byte(&bytes);

    size_t thumbnail_size = app0.thumbnail_width * app0.thumbnail_height * 3;
    app0.thumbnail_bytes = imj_read_bytes(&bytes, thumbnail_size);

    return app0;
}

Imj_DQT imj_parse_dqt(Imj_Bytes_View bytes) {
    Imj_DQT dqt = {0};

    byte info = imj_read_byte(&bytes);
    byte id = info & 0x0F;
    // NOTE(nic): maybe we should allow only 1 and 0 as valid
    byte precision = (((info & 0xF0) >> 4) == 0) ? 0 : 1;

    dqt.id = id;
    dqt.qt_bytes = imj_read_bytes(&bytes, 64 * (1 + precision));
    return dqt;
}

Imj_SOF0 imj_parse_sof0(Imj_Bytes_View bytes) {
    Imj_SOF0 sof0 = {0};
    sof0.data_precision = imj_read_byte(&bytes);
    sof0.image_width = imj_read_be_u16(&bytes);
    sof0.image_height = imj_read_be_u16(&bytes);
    sof0.components_count = imj_read_byte(&bytes);

    for (size_t i = 0; i < sof0.components_count; ++i) {
        Imj_Bytes_View comp_info = imj_read_bytes(&bytes, 3);

        byte id = imj_read_byte(&comp_info) - 1;
        byte sampling_factors = imj_read_byte(&comp_info);
        byte qt_number = imj_read_byte(&comp_info);

        sof0.components[i].id = id;
        sof0.components[i].vertical_sampling_factor = (sampling_factors & 0x0F);
        sof0.components[i].horizontal_sampling_factor = ((sampling_factors & 0xF0) >> 4);
        sof0.components[i].qt_number = qt_number;
    }
    return sof0;
}

Imj_DHT imj_parse_dht(Imj_Bytes_View bytes) {
    Imj_DHT dht = {0};

    byte info = imj_read_byte(&bytes);

    byte id = (info & 0x0F);
    Imj_HT_Type type = ((info & 0xF0) == 0) ? IMJ_HT_DC: IMJ_HT_AC;

    Imj_Bytes_View number_of_symbols = imj_read_bytes(&bytes, 16);

    byte symbols_buffer_size = 0;
    for (size_t i = 0; i < number_of_symbols.count; ++i) {
        symbols_buffer_size += number_of_symbols.data[i];
    }
    Imj_Bytes_View symbols = imj_read_bytes(&bytes, symbols_buffer_size);

    dht.id = id;
    dht.type = type;
    dht.symbols = symbols;
    return dht;
}

Imj_SOS imj_parse_sos(Imj_Bytes_View bytes) {
    Imj_SOS sos = {0};
    sos.components_count = imj_read_byte(&bytes);
    for (size_t i = 0; i < sos.components_count; ++i) {
        Imj_Comp_Id id = imj_read_byte(&bytes) - 1;
        byte ht_ids = imj_read_byte(&bytes);
        sos.components[i].id = id;
        sos.components[i].ac_ht_id = (ht_ids & 0x0F);
        sos.components[i].dc_ht_id = ((ht_ids & 0xF0) >> 4);
    }
    return sos;
}

#endif // IMAJE_IMPLEMTATION
