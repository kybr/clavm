// https://www.youtube.com/watch?v=L9KLnN0GczI (bisqwit)
// https://www.youtube.com/watch?v=GtQdIYUtAHg (viznut 1st iteration)
// https://www.youtube.com/watch?v=qlrs2Vorw2Y (viznut 2nd iteration)
// https://www.youtube.com/watch?v=tCRPUv8V22o (viznut 3rd iteration)
//

char viznut_music(int t, int s);

void process(double d, float* i, float* o) {
  // tune into a new bytebeat station every 2 seconds
  int which = (int)(d / 2) % 16;

  // simulate 8000 Hz playback rate
  d *= 8000.0 * 1.0;

  char l = viznut_music(d, which);
  char r = viznut_music(d, which);

  o[0] = l / 128.0 * 0.3;
  o[1] = r / 128.0 * 0.3;
}

// https://github.com/viznut/IBNIZ
// http://viznut.fi/texts_en/bytebeat_exploring_space.pdf
// https://github.com/phillipae/c_musical-oneliners
// http://viznut.fi/texts_fi/bytebeat_ja_demoskene.html
// http://viznut.fi/texts_en/bytebeat_deep_analysis.html
// http://viznut.fi/texts_en/bytebeat_algorithmic_symphonies.html
// http://canonical.org/~kragen/bytebeat/
// https://news.ycombinator.com/item?id=3063359

char viznut_music(int t, int s) {
  return t * ((t >> 12 | t >> 8) & 63 & t >> 4);
}
