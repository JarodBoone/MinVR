#ifndef VRDISPLAYGRAPHNODE_H_
#define VRDISPLAYGRAPHNODE_H_

namespace MinVR {

class VRDisplayGraphNode
{
public:
	virtual ~VRDisplayGraphNode() {}

	virtual void startRender() = 0;
	virtual void waitForRenderComplete() = 0;
	virtual void synchronize() = 0;
};

}

#endif
