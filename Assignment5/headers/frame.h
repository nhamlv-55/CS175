#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>
#if __GNUG__
#   include <tr1/memory>
#endif

#include "scenegraph.h"
#include "sgutils.h"

using namespace std;
using namespace tr1;

class Frame {
private:
  static const char SERIALIZATION_DELIMITER = ' ';
  vector< shared_ptr<SgRbtNode> > nodesInScene;
  vector<RigTForm> frameRBTs;

  /**
   * Creates a new Frame representing the arbitrary RBTs passed in.
   */
  Frame(shared_ptr<SgNode> root, vector<RigTForm> frameRBTs_) {
    /* Dumps the current scene into nodes. */
    dumpSgRbtNodes(root, nodesInScene);
    /* Directly sets the frame RBTs to what we want. */
    frameRBTs = frameRBTs_;
  }

  /**
   * Creates a new Frame representing the arbitrary RBTs passed in from the
   * nodes in scene that we want to use.
   */
  Frame(vector< shared_ptr<SgRbtNode> > nodesInScene_, vector<RigTForm> frameRBTs_) {
    /* Get the scene nodes. */
    nodesInScene = nodesInScene_;
    /* Directly sets the frame RBTs to what we want. */
    frameRBTs = frameRBTs_;
  }

  /**
   * Gets a vector to the nodes in the scene.
   */
  vector< shared_ptr<SgRbtNode> > getNodesInScene() {
    return nodesInScene;
  }

public:
  /**
   * Creates a new Frame representing the RBTs from the current scene.
   */
  Frame(shared_ptr<SgNode> root) {
    frameRBTs = vector<RigTForm>();

    /* Dumps the current scene into nodes. */
    dumpSgRbtNodes(root, nodesInScene);
    /* Iterates through the nodes and stores their corresponding RBTs into rbts. */
    for (int i = 0; i < nodesInScene.size(); i++) {
      frameRBTs.push_back(nodesInScene[i]->getRbt());
    }
  }

  /**
   * Sets the RBTs of the nodes into the scene to the RBTs stored in this Frame.
   */
  void showFrameInScene() {
    for (int i = 0; i < nodesInScene.size(); i++) {
      nodesInScene[i]->setRbt(frameRBTs[i]);
    }
  }

  /**
   * Returns the RBT-vector of this Frame.
   */
  vector<RigTForm> getRBTs() {
    return frameRBTs;
  }

  /**
   * Interpolates between the first frame and second frame, with the second
   * frame having a relative "weight" of alpha. Returns the interpolated frame.
   */
  static Frame interpolate(Frame firstFrame, Frame secondFrame, float alpha) {
    vector<RigTForm> firstFrameRBTs   = firstFrame .getRBTs();
    vector<RigTForm> secondFrameRBTs  = secondFrame.getRBTs();
    vector<RigTForm> interpolatedRBTs = vector<RigTForm>();

    for (int i = 0; i < firstFrameRBTs.size(); i++) {
      interpolatedRBTs.push_back(
        RigTForm(
          RigTForm::lerp(
            firstFrameRBTs[i].getTranslation(),
            secondFrameRBTs[i].getTranslation(),
            alpha
          ),
          RigTForm::slerp(
            firstFrameRBTs[i].getRotation(),
            secondFrameRBTs[i].getRotation(),
            alpha
          )
        )
      );
    }

    return Frame(firstFrame.getNodesInScene(), interpolatedRBTs);
  }

  /**
   * File-writable serialization string representing this frame.
   */
  string serialize() {
    stringstream s;
    for (int i = 0; i < frameRBTs.size(); i++) {
      s << frameRBTs[i].serialize();
      if (i != frameRBTs.size() - 1) s << SERIALIZATION_DELIMITER;
    }
    return s.str();
  }

  /**
   * Reads and returns a frame from the serialized version.
   */
  static Frame deserialize(shared_ptr<SgNode> root, string serialized) {
    vector<string> serialized_rbts = split(serialized, SERIALIZATION_DELIMITER);
    assert(serialized_rbts.size() > 0);

    vector<RigTForm> rbts = vector<RigTForm>();
    for (int i = 0; i < serialized_rbts.size(); i++) {
      rbts.push_back(RigTForm::deserialize(serialized_rbts[i]));
    }

    return Frame(root, rbts);
  }
};

#endif
