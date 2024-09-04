#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

typedef uint16_t u16;
typedef uint8_t byte;

typedef struct {
    byte *data;
    size_t count;
} Bytes;

typedef struct {
    const byte *data;
    size_t count;
} Bytes_View;

typedef enum {
    TYPE_SOI,
    TYPE_APP0,
    TYPE_APP15,
    TYPE_SOF0,
    TYPE_SOF1,
    TYPE_SOF2,
    TYPE_SOF3,
    TYPE_SOF5,
    TYPE_SOF6,
    TYPE_SOF7,
    TYPE_SOF9,
    TYPE_SOF10,
    TYPE_SOF11,
    TYPE_SOF13,
    TYPE_SOF14,
    TYPE_SOF15,
    TYPE_DHT,
    TYPE_DQT,
    TYPE_SOS,
    TYPE_JPG,
    TYPE_JPG0,
    TYPE_JPG13,
    TYPE_DAC,
    TYPE_DNL,
    TYPE_DRI,
    TYPE_DHP,
    TYPE_EXP,
    TYPE_RST0,
    TYPE_RST1,
    TYPE_RST2,
    TYPE_RST3,
    TYPE_RST4,
    TYPE_RST5,
    TYPE_RST6,
    TYPE_RST7,
    TYPE_TEM,
    TYPE_COM,
    TYPE_EOI,
} Header_Type;

const char *header_type_to_cstr(Header_Type type) {
    switch (type) {
    case TYPE_SOI: return "SOI";
    case TYPE_APP0: return "APP0";
    case TYPE_APP15: return "APP15";
    case TYPE_SOF0: return "SOF0";
    case TYPE_SOF1: return "SOF1";
    case TYPE_SOF2: return "SOF2";
    case TYPE_SOF3: return "SOF3";
    case TYPE_SOF5: return "SOF5";
    case TYPE_SOF6: return "SOF6";
    case TYPE_SOF7: return "SOF7";
    case TYPE_SOF9: return "SOF9";
    case TYPE_SOF10: return "SOF10";
    case TYPE_SOF11: return "SOF11";
    case TYPE_SOF13: return "SOF13";
    case TYPE_SOF14: return "SOF14";
    case TYPE_SOF15: return "SOF15";
    case TYPE_DHT: return "DHT";
    case TYPE_DQT: return "DQT";
    case TYPE_SOS: return "SOS";
    case TYPE_JPG: return "JPG";
    case TYPE_JPG0: return "JPG0";
    case TYPE_JPG13: return "JPG13";
    case TYPE_DAC: return "DAC";
    case TYPE_DNL: return "DNL";
    case TYPE_DRI: return "DRI";
    case TYPE_DHP: return "DHP";
    case TYPE_EXP: return "EXP";
    case TYPE_RST0: return "RST0";
    case TYPE_RST1: return "RST1";
    case TYPE_RST2: return "RST2";
    case TYPE_RST3: return "RST3";
    case TYPE_RST4: return "RST4";
    case TYPE_RST5: return "RST5";
    case TYPE_RST6: return "RST6";
    case TYPE_RST7: return "RST7";
    case TYPE_TEM: return "TEM";
    case TYPE_COM: return "COM";
    case TYPE_EOI: return "EOI";
    default: {
        fprintf(stderr, "Error: invalid header type: 0x%02X\n", type);
        exit(1);
    }
    }
}

Header_Type header_type_from_byte(byte byte) {
    switch (byte) {
    case 0xD8: return TYPE_SOI;
    case 0xE0: return TYPE_APP0;
    case 0xEF: return TYPE_APP15;
    case 0xC0: return TYPE_SOF0;
    case 0xC1: return TYPE_SOF1;
    case 0xC2: return TYPE_SOF2;
    case 0xC3: return TYPE_SOF3;
    case 0xC5: return TYPE_SOF5;
    case 0xC6: return TYPE_SOF6;
    case 0xC7: return TYPE_SOF7;
    case 0xC9: return TYPE_SOF9;
    case 0xCA: return TYPE_SOF10;
    case 0xCB: return TYPE_SOF11;
    case 0xCD: return TYPE_SOF13;
    case 0xCE: return TYPE_SOF14;
    case 0xCF: return TYPE_SOF15;
    case 0xC4: return TYPE_DHT;
    case 0xDB: return TYPE_DQT;
    case 0xDA: return TYPE_SOS;
    case 0xC8: return TYPE_JPG;
    case 0xF0: return TYPE_JPG0;
    case 0xFD: return TYPE_JPG13;
    case 0xCC: return TYPE_DAC;
    case 0xDC: return TYPE_DNL;
    case 0xDD: return TYPE_DRI;
    case 0xDE: return TYPE_DHP;
    case 0xDF: return TYPE_EXP;
    case 0xD0: return TYPE_RST0;
    case 0xD1: return TYPE_RST1;
    case 0xD2: return TYPE_RST2;
    case 0xD3: return TYPE_RST3;
    case 0xD4: return TYPE_RST4;
    case 0xD5: return TYPE_RST5;
    case 0xD6: return TYPE_RST6;
    case 0xD7: return TYPE_RST7;
    case 0x01: return TYPE_TEM;
    case 0xFE: return TYPE_COM;
    case 0xD9: return TYPE_EOI;
    default: {
        fprintf(stderr, "Error: invalid header type: 0x%02X\n", byte);
        exit(1);
    }
    }
}

Bytes read_file(const char *filepath) {
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

    Bytes bytes = {0};
    bytes.data = malloc(count * sizeof(byte));
    bytes.count = count;

    size_t read_count = fread(bytes.data, sizeof(byte), count, file);
    assert(count == read_count);
    return bytes;
}

Bytes_View read_bytes(Bytes_View *bytes, size_t count) {
    if (bytes->count < count) {
        fprintf(stderr, "Error: could not read %zu byte(s)\n", count);
        exit(1);
    }

    Bytes_View view = {0};
    view.data = bytes->data;
    view.count = count;

    bytes->data += count;
    bytes->count -= count;
    return view;
}

Bytes_View chop_bytes_until(Bytes_View *bytes, byte delimiter) {
    Bytes_View view = {0};
    view.data = bytes->data;

    size_t count = 0;
    while (view.data[count] != delimiter) {
        count += 1;
    }

    bytes->data += count;
    bytes->count -= count;

    view.count = count;
    return view;
}

byte read_byte(Bytes_View *bytes) {
    byte byte = bytes->data[0];
    bytes->data += 1;
    bytes->count -= 1;
    return byte;
}

typedef enum {
    PIXEL_ASPECT_RATIO = 0,
    PIXELS_PER_INCH = 1,
    PIXELS_PER_CM = 2,
} APP0_Unit_Type;

const char *density_unit_type_to_cstr(APP0_Unit_Type type) {
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

typedef struct {
    byte major_version_number;
    byte minor_version_number;

    APP0_Unit_Type density_unit_type;
    u16 x_density;
    u16 y_density;

    byte thumbnail_width;
    byte thumbnail_height;
    Bytes_View thumbnail_bytes;
} APP0;

typedef struct {
    byte id;
    Bytes_View qt_bytes;
} DQT;

typedef enum {
    COMP_Y = 1,
    COMP_CB = 2,
    COMP_CR = 3,
    COMP_I = 4,
    COMP_Q = 5,
} Component_Id;

const char *component_id_to_cstr(Component_Id id) {
    switch (id) {
    case COMP_Y: return "COMP_Y";
    case COMP_CB: return "COMP_CB";
    case COMP_CR: return "COMP_CR";
    case COMP_I: return "COMP_I";
    case COMP_Q: return "COMP_Q";
    default: {
        fprintf(stderr, "Error: invalid SOF0 component id: %d\n", id);
        exit(1);
    }
    }
}

typedef struct {
    Component_Id id;
    byte vertical_sampling_factor;
    byte horizontal_sampling_factor;
    byte qt_number;
} SOF0_Component;

#define COMPONENTS_CAP 5

typedef struct {
    byte data_precision; // In bits
    u16 image_width;
    u16 image_height;
    SOF0_Component components[COMPONENTS_CAP];
    byte components_count;
} SOF0;

typedef enum {
    HT_DC,
    HT_AC,
} HT_Type;

const char *ht_type_to_cstr(HT_Type type) {
    switch (type) {
    case HT_DC: return "HT_DC";
    case HT_AC: return "HT_AC";
    default: {
        fprintf(stderr, "Error: invalid DHT type: %d\n", type);
        exit(1);
    }
    }
}

typedef struct {
    byte id;
    HT_Type type;
    Bytes_View symbols;
} DHT;

typedef struct {
    Component_Id id;
    byte dc_ht_id;
    byte ac_ht_id;
} SOS_Component;

typedef struct {
    SOS_Component components[COMPONENTS_CAP];
    byte components_count;
} SOS;

u16 zigzag(Bytes_View bytes) {
    assert(bytes.count == 2);
    byte temp[2];
    temp[1] = read_byte(&bytes);
    temp[0] = read_byte(&bytes);
    return *(u16*)temp;
}

APP0 decode_app0(Bytes_View bytes) {
    APP0 app0 = {0};

    Bytes_View ident = read_bytes(&bytes, 5);

    byte expected_ident[5] = { 0x4A, 0x46, 0x49, 0x46, 0x00 };
    if (memcmp(ident.data, expected_ident, ident.count)) {
        fprintf(stderr, "Error: invalid JFIF APP0 marker segment: incorrect identifier sequence\n");
        exit(1);
    }

    app0.major_version_number = read_byte(&bytes);
    app0.minor_version_number = read_byte(&bytes);

    byte density_unit_byte = read_byte(&bytes);
    switch (density_unit_byte) {
    case 0: app0.density_unit_type = PIXEL_ASPECT_RATIO; break;
    case 1: app0.density_unit_type = PIXELS_PER_INCH; break;
    case 2: app0.density_unit_type = PIXELS_PER_CM; break;
    default: {
        fprintf(stderr, "Error: invalid JFIF APP0 marker segment: incorrect density unit value: %d\n", density_unit_byte);
        exit(1);
    }
    }

    app0.x_density = zigzag(read_bytes(&bytes, 2));
    app0.y_density = zigzag(read_bytes(&bytes, 2));

    if (app0.x_density == 0 || app0.y_density == 0) {
        fprintf(stderr, "Error: invalid JFIF APP0 marker segment: x density and y density cannot be zero\n");
        exit(1);
    }

    app0.thumbnail_width = read_byte(&bytes);
    app0.thumbnail_height = read_byte(&bytes);

    size_t thumbnail_size = app0.thumbnail_width * app0.thumbnail_height * 3;
    app0.thumbnail_bytes = read_bytes(&bytes, thumbnail_size);

    return app0;
}

DQT decode_dqt(Bytes_View bytes) {
    DQT dqt = {0};

    byte info = read_byte(&bytes);
    byte id = info & 0x0F;
    // NOTE(nic): maybe we should allow only 1 and 0 as valid
    byte precision = ((info & 0xF0) == 0) ? 0 : 1;

    dqt.id = id;
    dqt.qt_bytes = read_bytes(&bytes, 64 * (1 + precision));
    return dqt;
}

SOF0 decode_sof0(Bytes_View bytes) {
    SOF0 sof0 = {0};
    sof0.data_precision = read_byte(&bytes);
    sof0.image_width = zigzag(read_bytes(&bytes, 2));
    sof0.image_height = zigzag(read_bytes(&bytes, 2));
    sof0.components_count = read_byte(&bytes);

    for (size_t i = 0; i < sof0.components_count; ++i) {
        Bytes_View comp_info = read_bytes(&bytes, 3);

        byte id = read_byte(&comp_info);
        byte sampling_factors = read_byte(&comp_info);
        byte qt_number = read_byte(&comp_info);

        sof0.components[i].id = id;
        sof0.components[i].vertical_sampling_factor = (sampling_factors & 0x0F);
        sof0.components[i].horizontal_sampling_factor = (sampling_factors & 0xF0);
        sof0.components[i].qt_number = qt_number;
    }
    return sof0;
}

DHT decode_dht(Bytes_View bytes) {
    DHT dht = {0};

    byte info = read_byte(&bytes);

    byte id = (info & 0x07);
    HT_Type type = ((info & 0x08) == 0) ? HT_DC: HT_AC;

    Bytes_View number_of_symbols = read_bytes(&bytes, 16);

    byte symbols_buffer_size = 0;
    for (size_t i = 0; i < number_of_symbols.count; ++i) {
        symbols_buffer_size += number_of_symbols.data[i];
    }
    Bytes_View symbols = read_bytes(&bytes, symbols_buffer_size);

    dht.id = id;
    dht.type = type;
    dht.symbols = symbols;
    return dht;
}

SOS decode_sos(Bytes_View bytes) {
    SOS sos = {0};
    sos.components_count = read_byte(&bytes);
    for (size_t i = 0; i < sos.components_count; ++i) {
        Component_Id id = read_byte(&bytes);
        byte ht_ids = read_byte(&bytes);
        sos.components[i].id = id;
        sos.components[i].ac_ht_id = (ht_ids & 0x0F);
        sos.components[i].dc_ht_id = (ht_ids & 0xF0);
    }
    return sos;
}

int main(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: expected input filepath\n");
        exit(1);
    }
    const char *filepath = argv[1];

    Bytes image_bytes = read_file(filepath);
    Bytes_View bytes_view = {
        .data = image_bytes.data,
        .count = image_bytes.count,
    };

    Bytes_View header = read_bytes(&bytes_view, 2);

    byte expected_header[2] = { 0xFF, 0xD8 };
    if (memcmp(header.data, expected_header, header.count)) {
        fprintf(stderr, "Error: filepath '%s' is not a valid jpg\n", filepath);
        exit(1);
    }

    while (true) {
        byte segment_byte = read_byte(&bytes_view);
        if (segment_byte == 0xFF) {
            continue;
        }

        Header_Type header_type = header_type_from_byte(segment_byte);
        if (header_type == TYPE_EOI) {
            break;
        }

        u16 segment_size = zigzag(read_bytes(&bytes_view, 2)) - 2;
        Bytes_View segment_bytes = read_bytes(&bytes_view, segment_size);
        printf("%s (0x%02X): %d\n", header_type_to_cstr(header_type), segment_byte, segment_size);

        if (header_type == TYPE_APP0) {
            APP0 app0 = decode_app0(segment_bytes);
            printf("    version: %d.%02d\n", app0.major_version_number, app0.minor_version_number);
            printf("    density unit type: %s\n", density_unit_type_to_cstr(app0.density_unit_type));
            printf("    x density: %d\n", app0.x_density);
            printf("    y density: %d\n", app0.y_density);
            printf("    thubmanil width: %d\n", app0.thumbnail_width);
            printf("    thubmanil height: %d\n", app0.thumbnail_height);
            printf("    thubmanil size in bytes: %zu\n", app0.thumbnail_bytes.count);
        } else if (header_type == TYPE_DQT) {
            DQT dqt = decode_dqt(segment_bytes);
            printf("    id: %d\n", dqt.id);
            printf("    table size in bytes: %zu\n", dqt.qt_bytes.count);
        } else if (header_type == TYPE_SOF0) {
            SOF0 sof0 = decode_sof0(segment_bytes);
            printf("    data precision (in bits): %d\n", sof0.data_precision);
            printf("    image width: %d\n", sof0.image_width);
            printf("    image height: %d\n", sof0.image_height);
            printf("    components count: %d\n", sof0.components_count);
            for (size_t i = 0; i < sof0.components_count; ++i) {
                printf("    component %s:\n", component_id_to_cstr(sof0.components[i].id));
                printf("        horizontal sampling factor: %d\n", sof0.components[i].horizontal_sampling_factor);
                printf("        vertical sampling factor: %d\n", sof0.components[i].vertical_sampling_factor);
                printf("        quantization table number: %d\n", sof0.components[i].qt_number);
            }
        } else if (header_type == TYPE_DHT) {
            DHT dht = decode_dht(segment_bytes);
            printf("    id: %d\n", dht.id);
            printf("    ht type: %s\n", ht_type_to_cstr(dht.type));
            printf("    symbols size in bytes: %zu\n", dht.symbols.count);
        } else if (header_type == TYPE_SOS) {
            SOS sos = decode_sos(segment_bytes);
            printf("    components count: %d\n", sos.components_count);
            for (size_t i = 0; i < sos.components_count; ++i) {
                printf("    component %s:\n", component_id_to_cstr(sos.components[i].id));
                printf("        ac ht id: %d\n", sos.components[i].ac_ht_id);
                printf("        dc ht id: %d\n", sos.components[i].dc_ht_id);
            }

            size_t count = 0;
            while (count < bytes_view.count) {
                // TODO(nic): technically unsafe, do proper check later
                if (bytes_view.data[count] == 0xFF && bytes_view.data[count + 1] != 0x00) {
                    break;
                }
                count += 1;
            }
            Bytes_View compressed_data_bytes = read_bytes(&bytes_view, count);
            printf("image data size in bytes: %zu\n", compressed_data_bytes.count);
        }
    }

    return 0;
}
