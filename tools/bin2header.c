/* bin2header. Convert our raw PPC code into a fancy C header
 *
 * The xxx_macro portion gives the raw code as a simple comma seperated
 * list of bytes. This is handy if you want to insert a payload into a
 * static descriptor stored in the flash memory of an AVR microcontroller.
 * Its usage will look something like this:
 *
 * const uint8_t PROGMEM usb_descriptor {
 *   // descriptor header bytes,
 *   default_payload_macro,
 *   // descriptor trailer bytes,
 * }
 *
 */

#include <stdio.h>
#include <string.h>

static const char ifdef_guard_header[] = \
  "#ifndef __%s__\n" \
  "#define __%s__\n\n";

static const char ifdef_guard_footer[] = "\n#endif\n";

static const char macro_header_pic[] = "#define %s_macro_pic_%d \\\n ";

static const char macro_footer[] = "\n";

int main(int argc, char **argv)
{
  char buf[256];
  FILE *fi, *fo;
  int i, idx, r, last_byte;
  long file_size;

  if (argc < 4) {
    fprintf(stderr, "Usage: %s <raw> <c header> <array name>\n", argv[0]);
    return -1;
  }

  fi = fopen(argv[1], "rb");
  if (fi == NULL) {
    perror(argv[1]);
    return -2;
  }

  fo = fopen(argv[2], "w");
  if (fo == NULL) {
    perror(argv[2]);
    return -3;
  }

  fprintf(fo, ifdef_guard_header, argv[3], argv[3]);

  // obtain the file size
  fseek (fi , 0 , SEEK_END);
  file_size = ftell(fi) - 1;

    // rewind the file so it can be read again
	fseek(fi, 0, SEEK_SET);

	int j = 1;
	idx = 0;
	int maxDefine = 1280;
	int maxSize = 3808;
	int size = 0;
	int pad = 0;
	
	int sizeCurrent = 0;

	// print the macro version
	fprintf(fo, macro_header_pic, argv[3], j);

	while ((r = fread(buf, 1, sizeof(buf), fi)) > 0) {
		for (i = 0; i < r; i++) {
			fprintf(fo, " 0x%.2x", buf[i] & 0xff);
			size++;
			sizeCurrent++;

			last_byte = feof(fi) && i == r-1;
			if(idx == file_size) {
				last_byte = 1;
			}

			// dont print a comma after the last byte
			if(sizeCurrent < maxDefine && !last_byte) {
				fprintf(fo, ",");
			}

			// dont print the line continuation after the last byte
			if (++idx % 8 == 0) {
				if(sizeCurrent < (maxDefine - 1) && !last_byte) {
					fprintf(fo, " \\\n ");
				}

				if(sizeCurrent == maxDefine) {
					sizeCurrent = 0;
					j++;
					// print the macro version
					fprintf(fo, macro_footer);
					fprintf(fo, macro_footer);
					fprintf(fo, macro_header_pic, argv[3], j);
				}
			}
		}
	}

	pad = maxSize - size;

	int footed = 0;

	while(j < 3) {
		j++;
		// print the macro version
		fprintf(fo, macro_footer);
		fprintf(fo, macro_footer);
		fprintf(fo, macro_header_pic, argv[3], j);
		fprintf(fo, " 0x00");

		pad = pad - 1;

		fprintf(fo, macro_footer);
		fprintf(fo, macro_footer);

		footed = 1;
	}

	if(!footed) {
		fprintf(fo, macro_footer);
		fprintf(fo, macro_footer);
	}


	fprintf(fo, "#define %s_macro_pic_pad 0x%.4x\n", argv[3], pad);

	fprintf(fo, macro_footer);

	fprintf(fo, "#define %s_macro_pic_count %d\n", argv[3], j);

  fprintf(fo, ifdef_guard_footer);

  fclose(fi);
  fclose(fo);
  fprintf(stdout, "Header %s generated.\n", argv[3]);
  return 0;
}
