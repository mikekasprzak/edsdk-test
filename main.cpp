#include <stdio.h>
#include <EDSDK.h>

int main(int argc, char* argv[]) {
	int err;
	err = EdsInitializeSDK();
	
	EdsCameraListRef ecl;
	err = EdsGetCameraList(&ecl);
	
//	EdsUInt32 rc = EdsRetain(EdsCameraListRef);

	EdsUInt32 count = 0xffffffff;	
	err = EdsGetChildCount(ecl, &count);
	printf("Cameras found: %i\n", count);
	
	if (count == 0) {
		return 0;
	}
	
	printf("\n");

	EdsCameraRef camera;
	err = EdsGetChildAtIndex(ecl, 0, &camera);
	
	EdsDeviceInfo info;
	err = EdsGetDeviceInfo(camera, &info);
	printf("Port: %s\nDevice: %s\nSubType: %u\n", info.szPortName, info.szDeviceDescription, info.deviceSubType);
	
	printf("\nOpenning Session...\n");
	err = EdsOpenSession(camera);
	
	printf("Closing Session...\n");
	err = EdsCloseSession(camera);

	printf("Done\n");
	EdsTerminateSDK();
	return 0;
}
