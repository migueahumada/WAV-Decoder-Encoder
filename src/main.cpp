#include "Audio.h"

#ifdef _WIN32
  #define INPATH "../rsc/Woosh.wav"
  #define OUTPATH "../rsc/out2.wav"
  #define OUTPATH2 "../rsc/out3.wav"

  #define IMG_INPATH "../rsc/TestImage.bmp"
  #define IMG_OUTPATH "../rsc/OutImage.bmp"
#endif

#ifdef __APPLE__
  #define INPATH "../../rsc/SoundFile_01.wav"
  #define OUTPATH "../../rsc/out2.wav"
  #define OUTPATH2 "../../rsc/out3.wav"

  #define IMG_INPATH "../../rsc/TestImage.bmp"
  #define IMG_OUTPATH "../../rsc/OutImage.bmp"
#endif

#ifdef __UNIX__
  #define INPATH "../../rsc/SoundFile_01.wav"
  #define OUTPATH "../../rsc/out2.wav"
  #define OUTPATH2  "../../rsc/out3.wav"
#endif

                                               

int main()
{
  
  Audio audio;
  audio.decode(INPATH);
  audio.processAudio();

  audio.butterworth(FilterType::LOWPASS, 800.0f);
  audio.butterworth(FilterType::HIGHPASS, 100.0f);
  /*audio.biquad(FilterType::LOWPASS,800.0,0.707f);
  audio.biquad(FilterType::HIGHPASS, 100.0, 0.707f);*/
  
  audio.encode(OUTPATH);

  Audio newAudio;
  uint32 milliseconds = 250;
  newAudio.create(96000,16,4, milliseconds);
  newAudio.sine(0.5f,440.0f);
  newAudio.encode(OUTPATH2);



  return 0;
}
