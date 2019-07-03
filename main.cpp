#include <stdio.h>
#include <string.h>
#include <EDSDK.h>

#include <unistd.h>

// kEdsPropertyEvent_PropertyChanged


EdsError startLiveView(EdsCameraRef camera) {
	EdsError err = EDS_ERR_OK;
	
	// Get the output device for the live view image
	EdsUInt32 device;
	err = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);
	
	// PC live view starts by setting the PC as the output device for the live view image.
	if(err == EDS_ERR_OK) {
		device |= kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);
	}

	// A property change event notification is issued from the camera if property settings are made successfully.
	// Start downloading of the live view image once the property change notification arrives.
	return err;
}


EdsError endLiveView(EdsCameraRef camera) {
	EdsError err = EDS_ERR_OK;
	
	// Get the output device for the live view image
	EdsUInt32 device;
	err = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device );
	
	// PC live view ends if the PC is disconnected from the live view image output device.
		if(err == EDS_ERR_OK) {
		device &= ~kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);
	}
	
	return err;
}

EdsError downloadEvfData(EdsCameraRef camera) {
	EdsError err = EDS_ERR_OK;
	EdsStreamRef stream = NULL;
	EdsEvfImageRef evfImage = NULL;
	
	// Create memory stream.
	err = EdsCreateMemoryStream(0, &stream);
	
	// Create EvfImageRef.
	if (err == EDS_ERR_OK) {
		printf("* Stream created\n");
		err = EdsCreateEvfImageRef(stream, &evfImage);
	}
	
	// Download live view image data.
	if(err == EDS_ERR_OK) {
		printf("* Reference created\n");
		err = EdsDownloadEvfImage(camera, evfImage);
	}
	
	// Get the incidental data of the image.
	if (err == EDS_ERR_OK) {
		printf("* LiveView downloaded\n");

		// Get the zoom ratio
		EdsUInt32 zoom;
		EdsGetPropertyData(evfImage, kEdsPropID_Evf_ZoomPosition, 0 , sizeof(zoom), &zoom);
		
		// Get the focus and zoom border position
		EdsPoint point;
		EdsGetPropertyData(evfImage, kEdsPropID_Evf_ZoomPosition, 0 , sizeof(point), &point);
		
		printf("Zoom: %u  Focus: %i %i\n", zoom, point.x, point.y);
	
		//
		// Display image
		//

	//	EdsImageSource is = kEdsImageSrc_FullView;
		EdsImageSource is = kEdsImageSrc_Thumbnail;
	//	EdsImageSource is = kEdsImageSrc_Preview;
		EdsImageInfo info;
		//memset(&info, 0, sizeof(EdsImageInfo));
		err = EdsGetImageInfo(evfImage, is, &info);
		
		printf("Image: %u %u %u %u\n", info.width, info.height, info.numOfComponents, info.componentDepth);
		printf("Actual: %i %i %i %i\n", info.effectiveRect.point.x, info.effectiveRect.point.y, info.effectiveRect.size.width, info.effectiveRect.size.height);
	}
	
//	EdsTargetImageType tit = kEdsTargetImageType_RGB; // kEdsTargetImageType_RGB16
//	char data = new char[info.height*info.width*info.numOfComponents];
//	
//	err = EdsGetImage(evfImage, is, tit, );
//	
//	delete [] data;
	
	// Release stream
	if (stream != NULL) {
		EdsRelease(stream);
		stream = NULL;
	}
	
	// Release evfImage
	if (evfImage != NULL) {
		EdsRelease(evfImage);
		evfImage = NULL;
	}
	
	return err;
}

EdsError PropertyChange(EdsPropertyEvent inEvent, EdsPropertyID inPropertyID, EdsUInt32 inParam, EdsVoid *inContext) {
	printf("PC: %i 0x%04x %i (%p)\n", inEvent, inPropertyID, inParam, inContext);
	return EDS_ERR_OK;
}

EdsError CameraChange(EdsStateEvent inEvent, EdsUInt32 inEventData, EdsVoid *inContext) {
	printf("CC!!!\n");
	return EDS_ERR_OK;
}


int main(int argc, char* argv[]) {
	int err;
	err = EdsInitializeSDK();
	
	EdsCameraListRef ecl;
	err = EdsGetCameraList(&ecl);

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

	//
	printf("registering handler\n");
	EdsSetPropertyEventHandler(camera, kEdsPropertyEvent_PropertyChanged, PropertyChange, 0);
	//EdsSetCameraStateEventHandler(camera, kEdsPropertyEvent_PropertyChanged, CameraChange, 0);
	
	printf("\nOpenning Session...\n");
	err = EdsOpenSession(camera);
		
	printf("starting session...\n");
	err = startLiveView(camera);
	if (err != EDS_ERR_OK) {
		printf("problem starting live view\n");
	}
	
	printf("waiting...\n");
	sleep(1);
	printf("downloading.....\n");
	downloadEvfData(camera);
//	sleep(1);
	
	endLiveView(camera);
	printf("Closing Session...\n");
	err = EdsCloseSession(camera);

	printf("Done\n");
	EdsTerminateSDK();
	return 0;
}
