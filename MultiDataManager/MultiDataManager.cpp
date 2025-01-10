#include "MultiDataManager.h"

void MultiDataManager::GetTCPData(TCP::Host** _ppHost, TCP::Client** _ppClient, int* _pHostOrClient_)
{
    *_ppHost = host_;
    *_ppClient = client_;
    *_pHostOrClient_ = hostOrClient_;
}
