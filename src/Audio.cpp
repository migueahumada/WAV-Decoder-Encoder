#include "Audio.h"
#include <assert.h>
#include <limits>




void Audio::create(uint32 sampleRate, 
                   uint16 bitDepth, 
                   uint16 numChannels, 
                   size_t audioDataSize)
{

  m_sampleRate = sampleRate;
  m_bitsPerSample = bitDepth;
  m_numChannels = numChannels;
  m_dataSize = audioDataSize;
  

}

void Audio::create(uint32 sampleRate,
                   uint16 bitDepth,
                   uint16 numChannels,
                   uint32& durationInMS)
{
  m_sampleRate = sampleRate;
  m_bitsPerSample = bitDepth;
  m_numChannels = numChannels;
  
  float seconds = durationInMS * 0.001f;

  uint32 numSamples = m_sampleRate * numChannels * seconds;
  
  uint32 dataSize = numSamples * getBytesPerSample();

  m_dataSize = dataSize;
  
  m_data = new uint8[dataSize];

  memset(m_data,0,dataSize);
}

void Audio::decode(const String& filePath)
{
  std::fstream file(filePath, std::ios::binary | std::ios::in);

  if (!file.is_open())
  {
    return;
  }

  WAVE_HEADER waveHeader{0};

  readRiffChunk(file, waveHeader);

  readSubchunks(file, waveHeader);

  create(waveHeader.fmt.sampleRate,
         waveHeader.fmt.bitsPerSample,
         waveHeader.fmt.numChannels,
         static_cast<size_t>(waveHeader.data.subchunk2Size));

  file.close();

  printf("Succesfully decoded an audio file.\n");
}

void Audio::encode(const String& filePath)
{

  std::fstream file(filePath, std::ios::binary | std::ios::out);

  if (!file.is_open())
  {
    return;
  }

  WAVE_HEADER waveHeader{0};
  waveHeader.riff.chunkID = fourccRIFF;
  waveHeader.riff.chunkSize = static_cast<uint32>(sizeof(WAVE_HEADER) - 8 + m_dataSize);
  waveHeader.riff.format = fourccWAVE;

  waveHeader.fmt.subchunk1ID = fourccFMT;
  waveHeader.fmt.subchunk1Size = sizeof(FMT_SUBCHUNK) - 8;
  waveHeader.fmt.audioFormat = 1;
  waveHeader.fmt.numChannels = m_numChannels;
  waveHeader.fmt.sampleRate = m_sampleRate;
  waveHeader.fmt.byteRate = m_sampleRate * m_numChannels * getBytesPerSample();
  waveHeader.fmt.blockAlign = m_numChannels * getBytesPerSample();
  waveHeader.fmt.bitsPerSample = m_bitsPerSample;

  waveHeader.data.subchunk2ID = fourccDATA;
  waveHeader.data.subchunk2Size = static_cast<uint32>(m_dataSize);

  file.write(reinterpret_cast<char*>(&waveHeader), sizeof(waveHeader));

  file.write(reinterpret_cast<char*>(&*m_data), m_dataSize);

  file.close();

  printf("Succesfully encoded an audio file.\n");
}

/*
*              c0   c1
*    frame 0  | x | x |
*    frame 1  | x | x |
*    frame 2  | x | x |
* 
*/

// This is like using a get Pixel function
float Audio::getFrameSample(int channelIndex,
                            int frameIndex)
{
  assert(channelIndex < m_numChannels &&
         "Channel should not be greater than the number of channels");
  assert(channelIndex >= 0 && "Channel should not be less than 0");

  int samplePos = (frameIndex * m_numChannels + channelIndex) *  getBytesPerSample();
  
  float sampleValue = 0;

  if (m_bitsPerSample == 8)
  {
    int8 rawSample = *reinterpret_cast<int8*>(&m_data[samplePos]);

    sampleValue = static_cast<float>(rawSample) / std::numeric_limits<int8>::max();

  }

  if (m_bitsPerSample == 16)
  {
    int16 rawSample = *reinterpret_cast<int16*>(&m_data[samplePos]);

    sampleValue = static_cast<float>(rawSample)/ std::numeric_limits<int16>::max();

  }

  if (m_bitsPerSample == 32)
  {
    int32 rawSample = *reinterpret_cast<int32*>(&m_data[samplePos]);
    sampleValue = static_cast<float>(rawSample) / std::numeric_limits<int32>::max();

  }

  return sampleValue;

}

void Audio::setFrameSample(int channelIndex,
                           int frameIndex,
                           float sampleValue)
{
  
  assert(channelIndex < m_numChannels &&
    "Channel should not be greater than the number of channels");
  assert(channelIndex >= 0 && "Channel should not be less than 0");

  int samplePos = (frameIndex * m_numChannels + channelIndex) * getBytesPerSample();

  if (m_bitsPerSample == 8)
  {
    int8 out = static_cast<int8>(std::round(sampleValue * static_cast<float>(std::numeric_limits<int8>::max())));
    memcpy(&m_data[samplePos], &out, sizeof(int8));
  }

  if (m_bitsPerSample == 16)
  {

    int16 out = static_cast<int16>(std::round(sampleValue * static_cast<float>(std::numeric_limits<int16>::max())));
    memcpy(&m_data[samplePos], &out, sizeof(int16));
    
  }

  if (m_bitsPerSample == 32)
  {
    int32 out = static_cast<int32>(std::round(sampleValue * static_cast<float>(std::numeric_limits<int32>::max())));
    memcpy(&m_data[samplePos], &out, sizeof(int32));

  }

}


void Audio::processAudio()
{
  
  printf("The number of byte data is: %d\n", (int)m_dataSize);
  printf("The number of samples is: %d\n", (int)getTotalNumSamples());
  printf("The number of frames is: %d\n", (int)getTotalNumFrames());


  for (size_t frame = 0; frame < getTotalNumFrames(); ++frame)
  {
    for (size_t channel = 0; channel < getNumChannels(); ++channel)
    {
      /*float inSample = getFrameSample(channel, frame);
      setFrameSample(channel, frame, inSample * 0.5f);*/
    }
  }

  
}

void Audio::sine(float amp,
                 float freq,
                 float phase)
{

  for (uint32 frame = 0; frame < getTotalNumFrames(); ++frame)
  {
    for (uint32 channel = 0; channel < getNumChannels(); ++channel)
    {

      float outSample = amp * std::cosf(2.0f * PI * frame * freq / m_sampleRate + phase);

      setFrameSample(channel, frame, clamp(outSample,-1.0f,1.0f));
    }
  }
}

void Audio::phoneDial(float amp, float freq, float phase)
{
  for (uint32 frame = 0; frame < getTotalNumFrames(); ++frame)
  {
    for (uint32 channel = 0; channel < getNumChannels(); ++channel)
    {

      float outSample = amp * std::cosf(2*PI * frame * freq / m_sampleRate + phase)
        * std::cosf(2.0f * PI * frame * 100.0f / m_sampleRate + phase);

      setFrameSample(channel, frame, clamp(outSample, -1.0f, 1.0f));
    }
  }
}

/**
*
*   y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] - a1 * y[n-1] - a2 * y[n-2]
*/

void Audio::lowpass(float cutoff, float Q)
{
  
  

  const float w0 = 2.0f * PI * cutoff / m_sampleRate;
  float alpha = std::sinf(w0) / 2.0f * Q;

  float b0 = (1.0f - std::cosf(w0)) / 2.0f;
  float b1 = 1.0f - std::cosf(w0);
  float b2 = (1.0f - std::cosf(w0)) / 2.0f;

  float a0 = 1.0f + alpha;
  float a1 = -2.0f * std::cosf(w0);
  float a2 = 1.0f - alpha;

  a1 /= a0;
  a2 /= a0;
  b0 /= a0;
  b1 /= a0;
  b2 /= a0;

  Vector<float> x1(getNumChannels(), 0.0f);
  Vector<float> x2(getNumChannels(), 0.0f);
  Vector<float> y1(getNumChannels(), 0.0f);
  Vector<float> y2(getNumChannels(), 0.0f);

  for (int frame = 0; frame < getTotalNumFrames(); ++frame)
  {
    for (int channel = 0; channel < getNumChannels(); ++channel)
    {
      


      float x = getFrameSample(channel, frame);

      float y = b0 * x
        + b1 * x1[channel]
        + b2 * x2[channel]
        - a1 * y1[channel]
        - a2 * y2[channel];

      x2[channel] = x1[channel];
      x1[channel] = x;
      y2[channel] = y1[channel];
      y1[channel] = y;

      setFrameSample(channel, frame, clamp(y, -1.0f, 1.0f));
    }
  }

}

void Audio::highpass(float cutoff, float Q)
{
  const float w0 = 2.0f * PI * cutoff / m_sampleRate;
  float alpha = std::sinf(w0) / 2.0f * Q;

  float b0 = (1.0f + std::cosf(w0)) / 2.0f;
  float b1 = -1 * (1.0f + std::cosf(w0));
  float b2 = (1.0f + std::cosf(w0)) / 2.0f;

  float a0 = 1.0f + alpha;
  float a1 = -2.0f * std::cosf(w0);
  float a2 = 1.0f - alpha;

  a1 /= a0;
  a2 /= a0;
  b0 /= a0;
  b1 /= a0;
  b2 /= a0;

  Vector<float> x1(getNumChannels(), 0.0f);
  Vector<float> x2(getNumChannels(), 0.0f);
  Vector<float> y1(getNumChannels(), 0.0f);
  Vector<float> y2(getNumChannels(), 0.0f);

  for (int frame = 0; frame < getTotalNumFrames(); ++frame)
  {
    for (int channel = 0; channel < getNumChannels(); ++channel)
    {



      float x = getFrameSample(channel, frame);

      float y = b0 * x
        + b1 * x1[channel]
        + b2 * x2[channel]
        - a1 * y1[channel]
        - a2 * y2[channel];

      x2[channel] = x1[channel];
      x1[channel] = x;
      y2[channel] = y1[channel];
      y1[channel] = y;

      setFrameSample(channel, frame, clamp(y, -1.0f, 1.0f));
    }
  }
}

void Audio::bandpass(float cutoff, float Q)
{
  const float w0 = 2.0f * PI * cutoff / m_sampleRate;
  float alpha = std::sinf(w0) / 2.0f * Q;

  float b0 = std::sinf(w0) / 2.0f;
  float b1 = 0;
  float b2 = (std::sinf(w0) / 2.0f) * -1.0f;

  float a0 = 1.0f + alpha;
  float a1 = -2.0f * std::cosf(w0);
  float a2 = 1.0f - alpha;

  a1 /= a0;
  a2 /= a0;
  b0 /= a0;
  b1 /= a0;
  b2 /= a0;

  Vector<float> x1(getNumChannels(), 0.0f);
  Vector<float> x2(getNumChannels(), 0.0f);
  Vector<float> y1(getNumChannels(), 0.0f);
  Vector<float> y2(getNumChannels(), 0.0f);

  for (int frame = 0; frame < getTotalNumFrames(); ++frame)
  {
    for (int channel = 0; channel < getNumChannels(); ++channel)
    {



      float x = getFrameSample(channel, frame);

      float y = b0 * x
        + b1 * x1[channel]
        + b2 * x2[channel]
        - a1 * y1[channel]
        - a2 * y2[channel];

      x2[channel] = x1[channel];
      x1[channel] = x;
      y2[channel] = y1[channel];
      y1[channel] = y;

      setFrameSample(channel, frame, clamp(y, -1.0f, 1.0f));
    }
  }
}

void Audio::biquad(FilterType::E type, float cutoff, float Q)
{
  switch (type)
  {
  case FilterType::LOWPASS:
    lowpass(cutoff,Q);
    break;
  case FilterType::HIGHPASS:
    highpass(cutoff,Q);
    break;
  case FilterType::BANDPASS:
    bandpass(cutoff,Q);
    break;
  default:
    break;
  }
}

void Audio::butterworth(FilterType::E type, float cutoff)
{
  switch (type)
  {
  case FilterType::LOWPASS:
    lowpass(cutoff, 0.707f);
    break;
  case FilterType::HIGHPASS:
    highpass(cutoff, 0.707f);
    break;
  case FilterType::BANDPASS:
    bandpass(cutoff, 0.707f);
    break;
  default:
    break;
  }
}


void Audio::readRiffChunk(std::fstream& file,
                          WAVE_HEADER& waveHeader)
{
  uint32 tempBuffer;

  //IS RIFF?
  file.read(reinterpret_cast<char*>(&tempBuffer), sizeof(tempBuffer));
  if (tempBuffer != fourccRIFF)
  {
    return;
  }

  waveHeader.riff.chunkID = tempBuffer;

  //SIZE OF RIFF CHUNK
  file.read(reinterpret_cast<char*>(&tempBuffer), sizeof(tempBuffer));

  waveHeader.riff.chunkSize = tempBuffer;

  //IS FMT?
  file.read(reinterpret_cast<char*>(&tempBuffer), sizeof(tempBuffer));
  if (tempBuffer != fourccWAVE)
  {
    return;
  }

  waveHeader.riff.format = tempBuffer;
}

void Audio::readSubchunks(std::fstream& file,
                          WAVE_HEADER& waveHeader)
{

  //ITERATE THROUGH SUBCHUNKS
  while (file.peek() != EOF)
  {

    uint32 tempBuffer;
    file.read(reinterpret_cast<char*>(&tempBuffer), sizeof(tempBuffer));

    switch (tempBuffer)
    {
    case fourccFMT:

      file.seekg(-static_cast<std::streamoff>(sizeof(tempBuffer)), std::ios::cur);

      file.read(reinterpret_cast<char*>(&waveHeader.fmt), sizeof(waveHeader.fmt));

      break;

    case fourccDATA:

      file.seekg(-static_cast<std::streamoff>(sizeof(tempBuffer)), std::ios::cur);

      file.read(reinterpret_cast<char*>(&waveHeader.data), sizeof(waveHeader.data));

      m_data = new uint8[waveHeader.data.subchunk2Size];

      file.read(reinterpret_cast<char*>(&*m_data), waveHeader.data.subchunk2Size);
      

      break;

    default:
      file.read(reinterpret_cast<char*>(&tempBuffer), sizeof(tempBuffer));

      file.seekg(static_cast<std::streamoff>(tempBuffer), std::ios::cur);
      break;
    }
  }
}

