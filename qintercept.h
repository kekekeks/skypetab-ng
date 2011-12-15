#ifndef QINTERCEPT_H
#define QINTERCEPT_H
namespace skypetab
{
	void AddSignalIntercept(const char* className,const char*signalName, QObject*target, const char* targetSlot, const char* targetSignal);
}
#endif // QINTERCEPT_H
