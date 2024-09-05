#define IMAJE_IMPLEMTATION
#include "./imaje.h"

int main(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: expected input filepath\n");
        exit(1);
    }
    const char *filepath = argv[1];

    Imj_Bytes image_bytes = imj_read_file(filepath);
    Imj_Bytes_View bytes_view = {
        .data = image_bytes.data,
        .count = image_bytes.count,
    };

    Imj_Bytes_View header = imj_read_bytes(&bytes_view, 2);

    byte expected_header[2] = { 0xFF, 0xD8 };
    if (memcmp(header.data, expected_header, header.count)) {
        fprintf(stderr, "Error: filepath '%s' is not a valid jpg\n", filepath);
        exit(1);
    }

    while (true) {
        byte segment_byte = imj_read_byte(&bytes_view);
        if (segment_byte == 0xFF) {
            continue;
        }

        Imj_Header_Type header_type = imj_header_type_from_byte(segment_byte);
        if (header_type == IMJ_EOI) {
            break;
        }

        u16 segment_size = imj_read_be_u16(&bytes_view) - 2;
        Imj_Bytes_View segment_bytes = imj_read_bytes(&bytes_view, segment_size);
        printf("%s (0x%02X): %d\n", imj_header_type_to_cstr(header_type), segment_byte, segment_size);

        if (header_type == IMJ_APP0) {
            Imj_APP0 app0 = imj_parse_app0(segment_bytes);
            printf("    version: %d.%02d\n", app0.major_version_number, app0.minor_version_number);
            printf("    density unit type: %s\n", imj_density_unit_type_to_cstr(app0.density_unit_type));
            printf("    x density: %d\n", app0.x_density);
            printf("    y density: %d\n", app0.y_density);
            printf("    thubmanil width: %d\n", app0.thumbnail_width);
            printf("    thubmanil height: %d\n", app0.thumbnail_height);
            printf("    thubmanil size in bytes: %zu\n", app0.thumbnail_bytes.count);
        } else if (header_type == IMJ_DQT) {
            Imj_DQT dqt = imj_parse_dqt(segment_bytes);
            printf("    id: %d\n", dqt.id);
            printf("    table size in bytes: %zu\n", dqt.qt_bytes.count);
        } else if (header_type == IMJ_SOF0) {
            Imj_SOF0 sof0 = imj_parse_sof0(segment_bytes);
            printf("    data precision (in bits): %d\n", sof0.data_precision);
            printf("    image width: %d\n", sof0.image_width);
            printf("    image height: %d\n", sof0.image_height);
            printf("    components count: %d\n", sof0.components_count);
            for (size_t i = 0; i < sof0.components_count; ++i) {
                printf("    component %s:\n", imj_comp_id_to_cstr(sof0.components[i].id));
                printf("        horizontal sampling factor: %d\n", sof0.components[i].horizontal_sampling_factor);
                printf("        vertical sampling factor: %d\n", sof0.components[i].vertical_sampling_factor);
                printf("        quantization table number: %d\n", sof0.components[i].qt_number);
            }
        } else if (header_type == IMJ_DHT) {
            Imj_DHT dht = imj_parse_dht(segment_bytes);
            printf("    id: %d\n", dht.id);
            printf("    ht type: %s\n", imj_ht_type_to_cstr(dht.type));
            printf("    symbols size in bytes: %zu\n", dht.symbols.count);
        } else if (header_type == IMJ_SOS) {
            Imj_SOS sos = imj_parse_sos(segment_bytes);
            printf("    components count: %d\n", sos.components_count);
            for (size_t i = 0; i < sos.components_count; ++i) {
                printf("    component %s:\n", imj_comp_id_to_cstr(sos.components[i].id));
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
            Imj_Bytes_View compressed_data_bytes = imj_read_bytes(&bytes_view, count);
            printf("image data size in bytes: %zu\n", compressed_data_bytes.count);
        } else {
            fprintf(stderr, "Error: parsing of %s segment not implemented yet\n", imj_header_type_to_cstr(header_type));
            exit(1);
        }
    }

    return 0;
}
