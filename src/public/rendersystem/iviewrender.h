#include "irendercontext.h"

class IViewRendering
{
public:
	virtual void Init() = 0;
	virtual void Frame(SwapChainHandle_t hSwapChain) = 0;
};

extern IViewRendering *pViewRendering;