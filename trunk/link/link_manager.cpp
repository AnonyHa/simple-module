#include <map>
#include "packet_process.h"
#include "protocol.h"

using namespace std;

map<clsPacketDecrypt* ,proto_manager* > Dec2PtoManager;
map<proto_manager*, clsPacketEncrypt* > PtoManager2Enc;

bool RegisterDec2PtoManager(clsPacketDecrypt* Dec, proto_manager* PtoManager)
{
	if (Dec2PtoManager.count(Dec)) return false;
	Dec2PtoManager[Dec] = PtoManager;
	return true;
}

bool RegisterPtoManager2Enc(clsPacketEncrypt* Enc, proto_manager* PtoManager)
{
	if (PtoManager2Enc.count(PtoManager)) return false;
	PtoManager2Enc[PtoManager] = Enc;
	return true;
}

proto_manager* GetPtoManagerByDec(clsPacketDecrypt* Dec)
{
	if (!Dec2PtoManager.count(Dec)) return NULL;
	return Dec2PtoManager.count(Dec);
}

clsPacketEncrypt* GetEncByPtoManager(proto_manager* PtoManager)
{
	if (!PtoManager2Enc.count(PtoManager)) return NULL;
	return PtoManager2Enc[PtoManager];
}
