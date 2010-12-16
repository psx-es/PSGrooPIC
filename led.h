unsigned char ledStatus = 0;

void initLED() {
	output_bit(LEDR1, 1);
	output_bit(LEDR2, 1);
	output_bit(LEDR3, 1);
	output_bit(LEDG1, 1);
	output_bit(LEDG2, 1);
}

void blink0LED() {
	ledStatus =! ledStatus;

	output_bit(LEDR1, ledStatus);
	output_bit(LEDR2, ledStatus);
	output_bit(LEDR3, ledStatus);
	output_bit(LEDG1, ledStatus);
	output_bit(LEDG2, ledStatus);
}

void blink1LED() {
	output_bit(LEDR1, 0);
	output_bit(LEDR2, 0);
	output_bit(LEDR3, 0);
	output_bit(LEDG1, 1);
	output_bit(LEDG2, 1);
}

void blink2LED() {
	output_bit(LEDR1, 0);
	output_bit(LEDR2, 0);
	output_bit(LEDR3, 0);
	output_bit(LEDG1, 0);
	output_bit(LEDG2, 0);
}