// Examples of bytebeat
//
// https://www.youtube.com/watch?v=L9KLnN0GczI (bisqwit)
// https://www.youtube.com/watch?v=GtQdIYUtAHg (viznut 1st iteration)
// https://www.youtube.com/watch?v=qlrs2Vorw2Y (viznut 2nd iteration)
// https://www.youtube.com/watch?v=tCRPUv8V22o (viznut 3rd iteration)
//

// https://github.com/viznut/IBNIZ
// http://viznut.fi/texts_en/bytebeat_exploring_space.pdf
// https://github.com/phillipae/c_musical-oneliners
// http://viznut.fi/texts_fi/bytebeat_ja_demoskene.html
// http://viznut.fi/texts_en/bytebeat_deep_analysis.html
// http://viznut.fi/texts_en/bytebeat_algorithmic_symphonies.html
// http://canonical.org/~kragen/bytebeat/
// https://news.ycombinator.com/item?id=3063359
//

char bytebeat(int t, int which) {
  static int v = 0;

  switch (which) {
    case 1:  // viznut/viznut-music/1-viznut.c
      return t * ((t >> 12 | t >> 8) & 63 & t >> 4);

    case 2:  // viznut/viznut-music/2-tejeez.c
      return t * ((t >> 5 | t >> 8) >> (t >> 16));

    case 4:  // viznut/viznut-music/4-visy.c
      return t * ((t >> 9 | t >> 13) & 25 & t >> 6);

    case 5:  // viznut/viznut-music/5-tejeez.c
      return t * (t >> 11 & t >> 8 & 123 * t >> 3);

    case 6:  // viznut/viznut-music/6-visy.c
      return t *
             (t >> 8 * (t >> 15 | t >> 8) & (20 | (t >> 19) * 5 >> t | t >> 3));

    case 7:  // viznut/viznut-music/7-tejeez.c
      return ((-t & 4095) * (255 & t * (t & t >> 13)) >> 12) +
             (127 & t * (234 & t >> 8 & t >> 3) >> (3 & t >> 14));

    case 8:  // viznut/viznut-music/8-visy.c
      return t * (t >> ((t >> 9 | t >> 8)) & 63 & t >> 4);

    case 9:  // viznut/viznut-music/9-viznut.c
      return (t >> 6 | t | t >> (t >> 16)) * 10 + ((t >> 11) & 7);

    case 10:  // viznut/viznut-music/a-pyryp.c
      return v = (v >> 1) + (v >> 4) +
                 t * (((t >> 16) | (t >> 6)) & (69 & (t >> 9)));

    case 11:  // viznut/viznut-music/b-red-.c
      return (t | (t >> 9 | t >> 7)) * t & (t >> 11 | t >> 9);

    case 12:  // viznut/viznut-music/c-miiro.c
      return t * 5 & (t >> 7) | t * 3 & (t * 4 >> 10);

    case 13:  // viznut/viznut-music/d-viznut-xpansive-varjohukka.c
      return (t >> 7 | t | t >> 6) * 10 + 4 * (t & t >> 13 | t >> 6);

    case 14:  // viznut/viznut-music/e-skurk-raer.c
      return (t & 4096) ? ((t * (t ^ t % 255) | (t >> 4)) >> 1)
                        : (t >> 3) | ((t & 8192) ? t << 2 : t);

    case 15:  // viznut/viznut-music/f-xpansive-lost-in-space.c
      return ((t * (t >> 8 | t >> 9) & 46 & t >> 8)) ^ (t & t >> 13 | t >> 6);
  }

  return 0;
}

void process(double d, float* i, float* o) {
  // tune into a new bytebeat station every 2 seconds
  int which = (int)(d / 2) % 16;

  // simulate 8000 Hz playback rate
  d *= 8000.0 * 1.9;

  int t = d;

  char r = bytebeat(t, which);

  o[0] = r / 128.0 * 0.1;
  o[1] = r / 128.0 * 0.1;
}

