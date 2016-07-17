ofxKinectHitBoxes
====================
OF 0.9.0 . Visual Studio 2015 and/or XCode

<img src="https://github.com/pelayomendez/ofxKinectHitBoxes/blob/master/screenshoot.png" />

ofxKinectHitboxes integrates different OF addons to create a tool for easily implementing virtual hitboxes universes for interactive installations 
in a similar way that [Kinect Hitboxes DX11](https://vvvv.org/contribution/kinect-hitboxes-dx11) works with vvvv.

## Features

- Creation and management of Virtual Hitboxes universe.
- Support for Kinect V1 and V2.
- Event based system.

## Dependencies

[ofxKinectForWindows2](https://github.com/elliotwoods/ofxKinectForWindows2)  
Implementation of Kinect For Windows v2 API using COM (not KinectCommonBridge) by Elliot Woods.

[ofxKinectBlobFinder](https://github.com/mattfelsen/ofxKinectBlobFinder)  
3D blob finder. We use Matt Felsen's fork wich contains branches that works with ofxKinect and ofxKinectForWindows2.

[ofxPointInMesh](https://github.com/AmnonOwed/ofxPointInMesh)  
A straightforward C++ openFrameworks addon to check if a 3D point is inside a closed 3D ofMesh by Amnon Owed.

## License

[MIT License](http://en.wikipedia.org/wiki/MIT_License)
