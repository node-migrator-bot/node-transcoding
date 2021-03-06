#include <node.h>
#include <v8.h>
#include "utils.h"
#include "packetfifo.h"
#include "profile.h"
#include "taskoptions.h"
#include "hls/playlist.h"
#include "io/io.h"

#ifndef NODE_TRANSCODING_TASKCONTEXT
#define NODE_TRANSCODING_TASKCONTEXT

using namespace v8;

namespace transcoding {

typedef struct Progress_t {
  double    timestamp;
  double    duration;
  double    timeElapsed;
  double    timeEstimated;
  double    timeRemaining;
  double    timeMultiplier;
} Progress;

class TaskContext {
public:
  TaskContext(io::IOReader* input, Profile* profile, TaskOptions* options);
  virtual ~TaskContext();

public:
  // Occurs exclusively in a worker thread
  virtual int PrepareInput();
  virtual int PrepareOutput();
  AVStream* AddOutputStreamCopy(AVFormatContext* octx, AVStream* istream,
      int* pret);
  bool NextPacket(int* pret, Progress* progress, AVPacket& packet);
  bool WritePacket(int* pret, AVPacket& packet);
  virtual bool Pump(int* pret, Progress* progress);
  void End();

public:
  io::IOReader*       input;
  Profile*            profile;
  TaskOptions*        options;

  AVFormatContext*    ictx;
  AVFormatContext*    octx;

  AVBitStreamFilterContext* bitStreamFilter;

  PacketFifo*         fifo;
  bool                doneReading;
};

class SingleFileTaskContext : public TaskContext {
public:
  SingleFileTaskContext(io::IOReader* input, io::IOWriter* output,
      Profile* profile, TaskOptions* options);
  virtual ~SingleFileTaskContext();

  virtual int PrepareOutput();

protected:
  io::IOWriter*       output;
};

class LiveStreamingTaskContext : public TaskContext {
public:
  LiveStreamingTaskContext(io::IOReader* input, Profile* profile,
      TaskOptions* options);
  virtual ~LiveStreamingTaskContext();

  virtual int PrepareOutput();
  virtual bool Pump(int* pret, Progress* progress);

protected:
  hls::Playlist*      playlist;

  int                 segmentId;
  io::IOWriter*       segmentOutput;
};

}; // transcoding

#endif // NODE_TRANSCODING_TASKCONTEXT
