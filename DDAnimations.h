// DDAnimations.h
// This file contains all of the music-visualizing patterns and non-music-visualizing patterns, unless you want to create a new header file for those.

// 
void fullSparkles(int fadeSpeed) {
	maxNumOfSparkles = (height*width) / 5; // for a matrix
	//brightness = 255;
	addSparkles(0, NUM_STRIPS * NUM_LEDS_PER_STRIP, 2, .06, -50, 0, false);
	addSparkles(0, NUM_STRIPS * NUM_LEDS_PER_STRIP, 3, .1, -50, 35, false);
	addSparkles(0, NUM_STRIPS * NUM_LEDS_PER_STRIP, 4, .1, -50, 70, false);
	addSparkles(0, NUM_STRIPS * NUM_LEDS_PER_STRIP, 6, .1, -50, 105, false);
	addSparkles(0, NUM_STRIPS * NUM_LEDS_PER_STRIP, 9, .1, -50, 180, false);
	addSparkles(0, NUM_STRIPS * NUM_LEDS_PER_STRIP, 12, .1, -50, 200, false);
	addSparkles(0, NUM_STRIPS * NUM_LEDS_PER_STRIP, 14, .1, -50, 200, false);


  // Here we set the speed, proportional to the volume of the music
  // Constrain the volume (Sum of all FFT bins) between 0 and 1000
  int fade = constrain(volume, 0, 2000); 
  // Map speed from 0 to 40
  fade = map(fade, 0, 2000, 0, 40); 
  // Flip it, so the louder the music is, the smaller 'speed' is.
  // '46' sets the fastest speed to 6 (every 6 times cycles, shift the lights).
  // (Any lower and it's fast... too fast...)
  fade = 255 - fade;
  fadeAll(fade);
}

// Explodes a new circle on beats in the low frequency
void drawCircles() {
	// These arrays allow for a maximum of 5 circles to be drawn at once
	// TODO: create a struct for them
	static int radIndex = 0; // used to cycle through circles
	static double radius[5]; // radii of circles
	static int hues[5]; // allows each circle to have a different hue
	static double xCenter[5]; // center of circle on x-axis
	static double yCenter[5]; // center of circle on y-axis
	static int thickness = 1; // thickness of the circles
	static int delayCounter = 0; // how slow to increase radii of circles

	if (beatDetected[lowBeatBin] == 2) { // if a beat is detected in the lowBeatBin...
		//EVERY_N_MILLIS(5000) { // This timer can be used instead of beat detection to test this pattern
		radius[radIndex] = 0; // set radius of circle = 0
		hues[radIndex] = random8(); // set a random hue
		xCenter[radIndex] = random8(1, width - 1); // choose xCenter, 1 away from the outside
		yCenter[radIndex] = random8(3, 5); // choose yCenter, 1 away from the inside
		radIndex++; // increase the index by 1, so the next beat will change another circle's properties
	}
	if (radIndex == 5) radIndex = 0; // if index = 5 (6th value), reset to 0

	delayCounter++; // increase delay counter
	if (delayCounter > 5) { // every 5 cycles through this code...
		for (int i = 0; i < 5; i++) { // for all the possible circles...
			if (radius[i] < 25) { // if the radius is less than 25 (to prevent circles from growing infinitely big) ...
				radius[i] += 1; // increase by 1
				propagateCircle(CHSV(hues[i], 255, 255 - radius[i] * 20), xCenter[i], yCenter[i], radius[i], thickness); // draw the circle on LEDs
			}
		}
		delayCounter = 0; // reset delay counter
	}

	// fade all the LEDs in leds3
	fadeAll3(245);
}

// fallingColorCreation() creates the lights for fallingMusic().
void fallingColorCreation(int freq) {
	static int fallingBrightness;
	static int threshold = 20;
	if (spectrumValue[freq] > threshold) {
		// constrain value between 0 and average + 3*stDev, map it to 0 - 255
		fallingBrightness = map(constrain(spectrumValue[freq] - 100, 0, average[freq] + 3 * stDev[freq]), 0, average[freq] + 3 * stDev[freq], 0, 255);
		fallingBrightness = constrain(fallingBrightness, 0, 255);
		//TODO: Make this work off of palettes
		leds[coords[map(freq, 0, 15, 0, width - 1)][height - 1]] += CHSV(map(freq, 0, 15, 0, 220), 255, fallingBrightness);
	}
}

void fallingMusic() {

	// Fade the top row of LEDs instead of turning them off so if they aren't moved down, they don't blink away really quick
	for (int i = 0; i < width; i++)
		leds[coords[i][height - 1]].nscale8(170); // pretty fast fade for only the top row

	// For every FFT bin, map the brightness of the light to the spectrumValue
	// TODO: Figure out why this for loop makes the LEDs all jittery and broken
	//for (int i = 0; i < 14; i++) 
	//	fallingColorCreation(i);

	fallingColorCreation(0);
	fallingColorCreation(1);
	fallingColorCreation(2);
	fallingColorCreation(3);
	fallingColorCreation(4);
	fallingColorCreation(5);
	fallingColorCreation(6);
	fallingColorCreation(7);
	fallingColorCreation(8);
	fallingColorCreation(9);
	fallingColorCreation(10);
	fallingColorCreation(11);
	fallingColorCreation(12);
	fallingColorCreation(13);
	fallingColorCreation(14);
	fallingColorCreation(15);

	// Move all of the lights down
	EVERY_N_MILLIS(50) { // this slows the speed at which the lights fall
		for (int i = 0; i < width; i++) { // for all the rows except the top one
			for (int j = 0; j < height - 1; j++) { // for all the columns
				leds[coords[i][j]] = leds[coords[i][j - 1]]; // set the LED equal to the one above it
			}
		}
	}
}

//// These next 2 functions are for XY arrays that are larger than 10x10
//// This function sets bar lengths and top dot locations
int barLength[16] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }; 
int topDot[16] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
void spectrumAnalyzerBars(int freq, int pillar, bool vert) {
	int preBarLength;
	if (vert) preBarLength = constrain(map(spectrumValue[freq], 50, 600, 0, height - 2), 0, height - 2);
	else preBarLength = constrain(map(spectrumValue[freq], 50, 600, 0, width / 4), 0, (width / 4)); // '-1' levaes room for topDot

	if (preBarLength > barLength[pillar]) {
		barLength[pillar] = preBarLength;
		if (barLength[pillar] > topDot[pillar])
			topDot[pillar] = barLength[pillar] + 1;
	}
}

//// This code works when the matrix is a square
//// These spectrum analyzer bars can go in the classic vertical direction (dir = true), or out from the middle (dir = false)
void spectrumAnalyzer(bool dir) {
	static int hue;
	//hue++;
	// Fade
	fadeAll(230);

	static int barDropTimer;
	static int dotDropTimer;

	//for (int i = 0; i < 8; i++) {
	//	if (i != 8) // Don't ask me why this works, but if the LEDs are glitching out, removing the 8th bar fixed it... 
	//		spectrumAnalyzerBars(i, i, dir);
	//}

	spectrumAnalyzerBars(1, 0, dir);
	spectrumAnalyzerBars(2, 1, dir);
  spectrumAnalyzerBars(3, 2, dir);
	spectrumAnalyzerBars(4, 3, dir);
	spectrumAnalyzerBars(6, 4, dir);
  spectrumAnalyzerBars(7, 5, dir);
	spectrumAnalyzerBars(8, 6, dir);
  spectrumAnalyzerBars(9, 7, dir);
	spectrumAnalyzerBars(10, 8, dir);
	spectrumAnalyzerBars(12, 9, dir);
	spectrumAnalyzerBars(14, 10, dir);

	// Using Every_N_Mllis for these totally fucked up the LEDs
	if (barDropTimer > 3) {
		for (int i = 0; i < numFFTBins; i++) {
			if (barLength[i] > 1)
				barLength[i]--;
		}
		barDropTimer = 0;
	}
	barDropTimer++;

	// Making the dots fall
	if (dotDropTimer > 15) {
		for (int i = 0; i < numFFTBins; i++) {
			if (topDot[i] > 1)
				topDot[i]--;
		}
		dotDropTimer = 0;
	}
	dotDropTimer++;

	// Draw bars and dots
	if (dir) { // for vertical bars
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < barLength[i]; j++) {
				//leds[coords[i][j]] = CHSV(i * (230 / numFFTBins), 255, 250); // rainbow goes left to right
				leds[coords[i][j]] = CHSV(j * (230 / numFFTBins), 255, 250); // rainbow goes bottom to top
				leds[coords[i][topDot[i]]] = CHSV(0, 0, 150);
			}
		}
	}
	else { // for horizontal bars (if these are glitchingo out, comment out all the code just above this if/else that makes the dots fall, no idea why this fixes it...
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < barLength[i]; j++) {
				// To see different colors, a couple options are taking out 'hue' and changing 'j' to 'i'
				leds[coords[44 +     j][i]] = CHSV(hue + i * (230 / height), 255, 250); // bars to the right
				leds[coords[43    - j][i]] = CHSV(hue + i * (230 / height), 255, 250); // bars to the left
				leds[coords[0 + j][i]] = CHSV(hue + i * (230 / height), 255, 250); // bars to the right
				leds[coords[87 - j][i]] = CHSV(hue + i * (230 / height), 255, 250); // bars to the left
				// These 2 lines draw the white dots to the left and right, but I think it looks better without them
				//leds[coords[(width / 2 ) + topDot[i] - 1][i]] = CHSV(0, 0, 150); // dots to the right
				//leds[coords[(width / 2 ) - topDot[i]][i]] = CHSV(0, 0, 150); // dots to the left
			}
		}
	}
}

void placeHolder() {

}

void fallingLights_Double() {
	static int fftBins[8] = { 1, 2, 4, 6, 9, 11, 12, 14 };
	static int speed, timer;

	for (int i = 0; i < 8; i++) {
		int tempBrightness = constrain(spectrumValue[fftBins[i]], 0, 600);
		tempBrightness = map(tempBrightness, 0, 600, 0, 255);
		if (tempBrightness > 150) {
			leds[coords[31][i]] += CHSV(map(i, 0, 8, 0, 220)- 20, 255, tempBrightness);
			leds[coords[32][i]] += CHSV(map(i, 0, 8, 0, 220)- 20, 255, tempBrightness);
		}
	}

		speed = constrain(volume, 0, 1000); // constrain the volume between 0 and 1000
		speed = map(speed, 0, 1000, 0, 40); // map this from 0 to 40
		speed = speed * -1 + 46; // flip it, so the louder the music is, the smaller 'speed' is. '42' ensures that the fastest is speed = 2
	//speed = 6;
		if (timer > speed) { // if the timer variable is higher than the speed
			timer = 0; // reset the timer variable
			for (int i = 0; i < 31; i++) { // For every row except the top
				for (int j = 0; j < 8; j++) {  // For every column
					leds[coords[63 - i][j]] = leds[coords[ 63 - i - 1][j]]; // move the leds down by one
					leds[coords[i][j]] = leds[coords[ i + 1][j]]; 
				}
			}
			for (int i = 0; i < height; i++) {
				leds[coords[31][i]].nscale8(80);
				leds[coords[32][i]].nscale8(80);
			}
		}
		timer++; // always be incrementing the timer variable
}

void shiftingLights_Quadruple() {
	// Visualize these 8 FFT bins (corresponding to frequencies from low to high)
	static int fftBins[11] = { 1, 2, 3, 4, 6, 7, 8, 9, 11, 12, 13 }; 
  static int fftBins6[6] = {1, 2, 5, 8, 10, 12 };
	// Variables to control how fast the lights move
	static int speed, timer; 

	// This is where light is created in 4 different columns
	// For every row on the LEDs (height = 8)
	for (int i = 0; i < height; i++) {
		// Constrain the volume of the frequency so we can map it
		int tempBrightness = constrain(spectrumValue[fftBins[i]], 0, 600); 
		// Map the constrained value to a brightness value
		tempBrightness = map(tempBrightness, 0, 600, 0, 255);
		// Only light up LEDs if brightness is above 150. Prevents dim, flickering LEDs
		// This also only shows the louder sounds, making it easier to see them
		if (tempBrightness > 150) { 
			// Add (+=) color to the 4 columns where light originates
			// Choosing a color is as simple as using CHSV(hue, saturation, brightness)
			// Hue is mapped to the rainbow and shifted (- 20), 255 is fully saturated
			leds[coords[0][i]]  += CHSV(map(i, 0, height, 0, 220) - 20, 255, tempBrightness);
			leds[coords[43][i]] += CHSV(map(i, 0, height, 0, 220) - 20, 255, tempBrightness);
			leds[coords[44][i]] += CHSV(map(i, 0, height, 0, 220) - 20, 255, tempBrightness);
			leds[coords[87][i]] += CHSV(map(i, 0, height, 0, 220) - 20, 255, tempBrightness);
		}
	}

// EXPERIMENTAL
//for (int i = 0; i < 5; i++) {
//    // Constrain the volume of the frequency so we can map it
//    int tempBrightness = constrain(spectrumValue[fftBins6[i]], 0, 600); 
//    // Map the constrained value to a brightness value
//    tempBrightness = map(tempBrightness, 0, 600, 0, 255);
//    // Only light up LEDs if brightness is above 150. Prevents dim, flickering LEDs
//    // This also only shows the louder sounds, making it easier to see them
//    if (tempBrightness > 150) { 
//      // Add (+=) color to the 4 columns where light originates
//      // Choosing a color is as simple as using CHSV(hue, saturation, brightness)
//      // Hue is mapped to the rainbow and shifted (- 20), 255 is fully saturated
//      leds[coords[0][4 - i]]  += CHSV(map(i, 0, 5, 0, 220) - 20, 255, tempBrightness);
//      leds[coords[43][4 - i]] += CHSV(map(i, 0, 5, 0, 220) - 20, 255, tempBrightness);
//      leds[coords[44][4 - i]] += CHSV(map(i, 0, 5, 0, 220) - 20, 255, tempBrightness);
//      leds[coords[87][4 - i]] += CHSV(map(i, 0, 5, 0, 220) - 20, 255, tempBrightness);
//      leds[coords[0][6 + i]]  += CHSV(map(i, 0, 5, 0, 220) - 20, 255, tempBrightness);
//      leds[coords[43][6 + i]] += CHSV(map(i, 0, 5, 0, 220) - 20, 255, tempBrightness);
//      leds[coords[44][6 + i]] += CHSV(map(i, 0, 5, 0, 220) - 20, 255, tempBrightness);
//      leds[coords[87][6 + i]] += CHSV(map(i, 0, 5, 0, 220) - 20, 255, tempBrightness);
//    }
//  }
//  int tempBrightness = constrain(spectrumValue[fftBins6[0]], 0, 600); 
//  tempBrightness = map(tempBrightness, 0, 600, 0, 255);
//  if (tempBrightness > 150) {
//    leds[coords[0][5]]  += CHSV(0 - 20, 255, tempBrightness);
//    leds[coords[43][5]] += CHSV(0 - 20, 255, tempBrightness);
//    leds[coords[44][5]] += CHSV(0 - 20, 255, tempBrightness);
//      leds[coords[87][5]] += CHSV(0 - 20, 255, tempBrightness);
//  }
    
  
	// Here we set the speed, proportional to the volume of the music
	// Constrain the volume (Sum of all FFT bins) between 0 and 1000
	speed = constrain(volume, 0, 1000); 
	// Map speed from 0 to 40
	speed = map(speed, 0, 1000, 0, 40); 
	// Flip it, so the louder the music is, the smaller 'speed' is.
	// '46' sets the fastest speed to 6 (every 6 times cycles, shift the lights).
	// (Any lower and it's fast... too fast...)
	speed = speed * -1 + 43; 

	// Finally, shift the LEDs and fade the originating columns
	// If the timer variable is higher than the speed
	if (timer > speed) { 
		// Reset the timer variable
		timer = 0; 
		// For every column we want the lights to move
		// ( (88 - 4) / 4 = 22 ) 88 - 4 because light orignates on 4 columns
		for (int i = 0; i < 21; i++) {
			// For each column
			for (int j = 0; j < height; j++) {
				// Shift the LEDs by 1, using the coordinate system coords[x][y]
				leds[coords[22 + i][j]] = leds[coords[22 + i + 1][j]];
				leds[coords[21 - i][j]] = leds[coords[21 - i - 1][j]];
				leds[coords[65 - i][j]] = leds[coords[65 - i - 1][j]];
				leds[coords[66 + i][j]] = leds[coords[66 + i + 1][j]];
			}
		}
		// Fade all of the LEDs where light originates after they're shifted
		for (int i = 0; i < height; i++) {
			leds[coords[43][i]].nscale8(80);
			leds[coords[44][i]].nscale8(80);
			leds[coords[0 ][i]].nscale8(80);
			leds[coords[87][i]].nscale8(80);
		}
	}
	timer++; // Always increment the timer variable
	
} // end shiftingLights_Quadruple()

int lines[10]; // This array holds the locations of the lines we want to draw
int lineHue[10]; // This array holds the locations of the lines we want to draw
int linesInd; // This index variable keeps us moving in lines[] so we don't overwrite a value
void sweep() {

	// Move the lines
	EVERY_N_MILLIS(60) { // Every 200 millis, increment all the line locations
		for (int i = 0; i < 10; i++) // This for loop goes through the whole lines[] array
			if (lines[i] < height)
				lines[i]++; // Increment each line
	}

	// Initialize new lines
	//EVERY_N_SECONDS(1) { // Every 1 second, initialize a new line at 0
	if (beatDetected[2] == 2) {
		lines[linesInd] = 0; // Initialize a line at 0
		lineHue[linesInd] = random8();
		linesInd++; // Move to the next spot in lines[] so the next line we write in 1 second writes to a spot that's not on the matrix
		if (linesInd == 10) linesInd = 0; // The lines array is 10 items long, 0 - 9, so when it reaches 10, set it back to 0.
	}

	// Draw the lines
	for (int i = 0; i < 10; i++) { // This for loop goes through all of the lines[] array
		if (lines[i] < height) { // We only want to draw it if the line is still on the matrix
			for (int j = 0; j < width; j++) // For the full width of the matrix
				//leds[coords[j][7 - lines[i]]] = CHSV(map(lines[i], 0, 7, 0, 220), 255, 255); // Apply color to the entire line.
				leds[coords[j][11 - lines[i]]] = CHSV(lineHue[i], 255, 100); // Apply color to the entire line.
		}
	}

	// Always be fading
	fadeAll(200);
}

void sparklesAndSweeps() {
	// generate sweep lines on beats at 64/8 = 8, so 0 - 7 , 16 - 23, 32 - 39, 48 - 55,
	// generate sparkles in the zones   64/8 = 8, so 8 - 15, 24 - 31, 40 - 47, 56 - 63
	static int speed, timer;
	static byte tempByte;
	static byte bigPos;

	if (beatDetected[lowBeatBin] == 2) {
		bigPos = bigPos + 4;
		CHSV tempColor = CHSV(random8(), 255, 100);
		for (int i = 0; i < 8; i++) {
			tempByte = 0*4 + bigPos + i*4;
			leds[coords[tempByte / 4][7]] = tempColor;
			Serial.println(tempByte);
			//tempByte = 16*4 + bigPos + i*4;
			//leds[coords[tempByte / 4][7]] = tempColor;
			//tempByte = 32*4 + bigPos + i*4;
			//leds[coords[tempByte / 4][7]] = tempColor;
			//tempByte = 48*4 + bigPos + i*4;
			//leds[coords[tempByte / 4][7]] = tempColor;


			//leds[coords[0 + i][7]] = tempColor;
			//leds[coords[16 + i][7]] = tempColor;
			//leds[coords[32 + i][7]] = tempColor;
			//leds[coords[48 + i][7]] = tempColor;
		}
	}

	int numOfSparkles = constrain(spectrumValue[12] - 50, 0, 600);
	numOfSparkles = map(numOfSparkles, 0, 600, 0, 10);
	for (int i = 0; i < numOfSparkles; i++) {
		int zone = random8(0, 4);
		int spot = random8(0, 8);
		CHSV tempColor = CHSV(0, 0, 30);
		switch (zone) {
		case 0:
			leds[coords[8 + spot][7]] = tempColor;
			break;
		case 1:
			leds[coords[24 + spot][7]] = tempColor;
			break;
		case 2:
			leds[coords[40  + spot][7]] = tempColor;
			break;
		case 3:
			leds[coords[56 + spot][7]] = tempColor;
			break;
		}
	}

	speed = constrain(volume, 0, 1000); // constrain the volume between 0 and 1000
	speed = map(speed, 0, 1000, 0, 40); // map this from 0 to 40
	speed = speed * -1 + 45; // flip it, so the louder the music is, the smaller 'speed' is. '42' ensures that the fastest is speed = 2

	if (timer > speed) { // if the timer variable is higher than the speed
		timer = 0; // reset the timer variable
		for (int i = 0; i < 7; i++) { // For every row except the top
			for (int j = 0; j < 64; j++) {  // For every column
				leds[coords[j][i]] = leds[coords[j][i + 1]]; // move the leds down by one
			}
		}
		for (int i = 0; i < width; i++) {
			leds[coords[i][7]].nscale8(80);
			leds[coords[i][7]].nscale8(80);
		}
	}
	timer++; // always be incrementing the timer variable

	delay(20);
}

void radar() {
	static byte bigPos;
	static byte pos;


	static int speed, timer;
	CRGB lows, mids, highs;
	CRGB radarLine[8];
	CRGB radarColor;
	static int addRed, addGreen, addBlue;

	addRed += map(constrain(spectrumValue[lowAveBin], 0, 600), 0, 600, 0, 10);
	addGreen += map(constrain(spectrumValue[midAveBin], 0, 600), 0, 600, 0, 10);
	addBlue += map(constrain(spectrumValue[highAveBin], 0, 600), 0, 600, 0, 10);
	if (beatDetected[lowBeatBin] == 2) addRed += 50;
	if (beatDetected[midBeatBin] == 2) addGreen += 50;
	if (beatDetected[highBeatBin] == 2) addBlue += 50;

	//for (int i = 0; i < 8; i++) {
		radarColor.r = constrain(radarColor.r + addRed, 20, 255);
		radarColor.g = constrain(radarColor.g + addGreen, 20, 255);
		radarColor.b = constrain(radarColor.b + addBlue, 20, 255);
		//radarLine[i].r += addRed;
		//radarLine[i].g += addGreen;
		//radarLine[i].b += addBlue;
	//}

	addRed = constrain(addRed - 5, -20, 20);
	addGreen = constrain(addGreen - 5, -20, 20);
	addBlue = constrain(addBlue - 5, -20, 20);
	

	speed = constrain(volume, 0, 1000); // constrain the volume between 0 and 1000
	speed = map(speed, 0, 1000, 0, 40); // map this from 0 to 40
	speed = speed * -1 + 50; // flip it, so the louder the music is, the smaller 'speed' is. '42' ensures that the fastest is speed = 2

	if (timer > speed) {
		timer = 0;
		bigPos += 4;
		pos = bigPos / 4;
		for (int i = 0; i < height; i++)
			leds[coords[pos][i]] = radarColor;
			//leds[coords[pos][i]] = radarLine[i];
		fadeAll(240);
	}
	timer++;


	//Serial.print(radarLine[0].r);
	//Serial.print("\t");
	//Serial.print(addRed);
	//Serial.print("\t");
	//Serial.print(radarLine[0].g);
	//Serial.print("\t");
	//Serial.print(addGreen);
	//Serial.print("\t");
	//Serial.print(radarLine[0].b);
	//Serial.print("\t");
	//Serial.println(addBlue);

	Serial.print(radarColor.r);
	Serial.print("\t");
	Serial.print(addRed);
	Serial.print("\t");
	Serial.print(radarColor.g);
	Serial.print("\t");
	Serial.print(addGreen);
	Serial.print("\t");
	Serial.print(radarColor.b);
	Serial.print("\t");
	Serial.println(addBlue);

	delay(10);
}


void topRowSparklesFalling() {
	static int fftBins[8] = { 1, 2, 4, 6, 9, 11, 12, 14 };

	for (int i = 0; i < 8; i++) {
		int tempNumSparkles = constrain(spectrumValue[fftBins[i]], 0, 600);
		tempNumSparkles = map(tempNumSparkles, 0, 600, 0, 10);
		for (int j = 0; j < tempNumSparkles; j++) {
			int pos = random8(0, 8);
			leds[coords[pos + i * 8][7]] = CHSV(map(i, 0, 15, 0, 220), 255, 255);
		}
	}
}

void shiftingVolumeMeter() {
	// Visualize these 8 FFT bins (corresponding to frequencies from low to high)
	static int fftBins[8] = { 1, 2, 4, 6, 9, 11, 12, 14 };
	// Variables to control how fast the lights move
	static int speed, timer;
	static int lowHeight, midHeight, highHeight;
	static int tempHeight, volHeight;

	//lowHeight = map(constrain(spectrumValue[lowAveBin], 0, 600), 0, 600, 0, height);
	//midHeight = map(constrain(spectrumValue[midAveBin], 0, 600), 0, 600, 0, height);
	//highHeight = map(constrain(spectrumValue[highAveBin], 0, 600), 0, 600, 0, height);

	//for (int i = 0; i < lowHeight; i++) {
	//	leds[coords[31][i]] += CRGB(map(lowHeight, 0, height, 0, 255), 0, 0);
	//	leds[coords[32][i]] += CRGB(map(lowHeight, 0, height, 0, 255), 0, 0);
	//}
	//for (int i = 0; i < midHeight; i++) {
	//	leds[coords[31][i]] += CRGB(0, map(midHeight, 0, height, 0, 255), 0);
	//	leds[coords[32][i]] += CRGB(0, map(midHeight, 0, height, 0, 255), 0);
	//}
	//for (int i = 0; i < highHeight; i++) {
	//	leds[coords[31][i]] += CRGB(0, 0, map(highHeight, 0, height, 0, 255));
	//	leds[coords[32][i]] += CRGB(0, 0, map(highHeight, 0, height, 0, 255));
	//}

	tempHeight = map(constrain(volume, 0, 6000), 0, 6000, 0, height);
	if (tempHeight > volHeight) volHeight = tempHeight;
	for (int i = 0; i < volHeight; i++) {
		leds[coords[31][i]] += CHSV(hue, 255, 255);
		leds[coords[32][i]] += CHSV(hue, 255, 255);
	}

	EVERY_N_MILLIS(40) {
		volHeight--;
		hue++;
	}
	Serial.println(volHeight);


	speed = constrain(volume, 0, 1000); // constrain the volume between 0 and 1000
	speed = map(speed, 0, 1000, 0, 40); // map this from 0 to 40
	speed = speed * -1 + 46; // flip it, so the louder the music is, the smaller 'speed' is. '42' ensures that the fastest is speed = 2
	speed = 6;
	if (timer > speed) { // if the timer variable is higher than the speed
		timer = 0; // reset the timer variable
		for (int i = 0; i < 31; i++) { // For every row except the top
			for (int j = 0; j < 8; j++) {  // For every column
				leds[coords[63 - i][j]] = leds[coords[63 - i - 1][j]]; // move the leds down by one
				leds[coords[i][j]] = leds[coords[i + 1][j]];
			}
		}
		for (int i = 0; i < height; i++) {
			leds[coords[31][i]].nscale8(80);
			leds[coords[32][i]].nscale8(80);
		}
	}
	timer++; // always be incrementing the timer variable

	delay(5);

} // end shiftingLights_Quadruple()

void fourMetersRising() {
	static int meterSize[4], tempMeterSize[4];

	// Create sizes
	tempMeterSize[0] = map(constrain(spectrumValue[2], 0, 600), 0, 600, 0, 8);
	if (tempMeterSize[0] > meterSize[0]) meterSize[0] = tempMeterSize[0];

	tempMeterSize[1] = map(constrain(spectrumValue[6], 0, 600), 0, 600, 0, 8);
	if (tempMeterSize[1] > meterSize[1]) meterSize[1] = tempMeterSize[1];

	tempMeterSize[2] = map(constrain(spectrumValue[9], 0, 600), 0, 600, 0, 8);
	if (tempMeterSize[2] > meterSize[2]) meterSize[2] = tempMeterSize[2];

	tempMeterSize[3] = map(constrain(spectrumValue[13], 0, 600), 0, 600, 0, 8);
	if (tempMeterSize[3] > meterSize[3]) meterSize[3] = tempMeterSize[3];

	// Draw meters
	for (int i = 0; i < meterSize[0]; i++) {
		leds[coords[8 - i][0]] += CHSV(0, 255, 255);
		leds[coords[8 + i][0]] += CHSV(0, 255, 255);
	}
	
	for (int i = 0; i < meterSize[1]; i++) {
		leds[coords[23 - i][0]] += CHSV(64, 255, 255);
		leds[coords[23 + i][0]] += CHSV(64, 255, 255);
	}

	for (int i = 0; i < meterSize[2]; i++) {
		leds[coords[39 - i][0]] += CHSV(128, 255, 255);
		leds[coords[39 + i][0]] += CHSV(128, 255, 255);
	}

	for (int i = 0; i < meterSize[3]; i++) {
		leds[coords[55 - i][0]] += CHSV(194, 255, 255);
		leds[coords[55 + i][0]] += CHSV(194, 255, 255);
	}

	EVERY_N_MILLIS(40) for (int i = 0; i < 4; i++) meterSize[i]--;


	EVERY_N_MILLIS(10) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < 7; j++) {
				leds[coords[i][7 - j]] = leds[coords[i][7 - j - 1]];
			}
		}
	}

	fadeAll(200);

	delay(5);
}

void middleOut() {
    static int fftBins[11] = { 1, 2, 3, 4, 6, 7, 8, 9, 11, 12, 13 }; 
  // Variables to control how fast the lights move
  static int speed, timer; 

  // This is where light is created in 8 rows on y = 5
  // For every FFT Bin being displayed (11)
  for (int i = 0; i < 11; i++) {
    // Constrain the volume of the frequency so we can map it
    int tempBrightness = constrain(spectrumValue[fftBins[i]], 0, 600); 
    // Map the constrained value to a brightness value
    tempBrightness = map(tempBrightness, 0, 600, 0, 255);
    // Only light up LEDs if brightness is above 150. Prevents dim, flickering LEDs
    // This also only shows the louder sounds, making it easier to see them
    if (tempBrightness > 150) {
      for (int j = 0; j < 8; j++) {
      // Add (+=) color to the 4 columns where light originates
      // Choosing a color is as simple as using CHSV(hue, saturation, brightness)
      // Hue is mapped to the rainbow and shifted (- 20), 255 is fully saturated
      leds[coords[j*11 + i][5]]  += CHSV(map(i, 0, height, 0, 220) - 20, 255, tempBrightness);
      }
    }
  }    
  
  // Here we set the speed, proportional to the volume of the music
  // Constrain the volume (Sum of all FFT bins) between 0 and 1000
  speed = constrain(volume, 0, 2000); 
  // Map speed from 0 to 40
  speed = map(speed, 0, 2000, 0, 40); 
  // Flip it, so the louder the music is, the smaller 'speed' is.
  // '46' sets the fastest speed to 6 (every 6 times cycles, shift the lights).
  // (Any lower and it's fast... too fast...)
  speed = speed * -1 + 42; 

  // Finally, shift the LEDs and fade the originating columns
  // If the timer variable is higher than the speed
  if (timer > speed) { 
    // Reset the timer variable
    timer = 0; 
    // For every segment we want the lights to move
    for (int i = 0; i < width; i++) {
      // For each segment
      for (int j = 0; j < 5; j++) { // 5 on each side of the center strip
        // Shift the LEDs by 1, using the coordinate system coords[x][y]
        leds[coords[i][height - 1 - j]] = leds[coords[i][height - 2 - j]];
        leds[coords[i][j]] = leds[coords[i][j + 1]];
        
//        leds[coords[22 + i][j]] = leds[coords[22 + i + 1][j]];
//        leds[coords[21 - i][j]] = leds[coords[21 - i - 1][j]];
//        leds[coords[65 - i][j]] = leds[coords[65 - i - 1][j]];
//        leds[coords[66 + i][j]] = leds[coords[66 + i + 1][j]];
      }
    }
    // Fade all of the LEDs where light originates after they're shifted
    for (int i = 0; i < width; i++) {
      leds[coords[i][5]].nscale8(80);
    }
  }
  timer++; // Always increment the timer variable

  fadeAll(240);
  
} // end shiftingLights_Quadruple()
