#include <iostream>
#include <vigra/multi_array.hxx>

#include "net_imglib2_vigra_VigraWrapper.h"
#include "VigraWrapper.hxx"

#include <vigra/multi_convolution.hxx>




#include <vigra/edgedetection.hxx>
#include <vigra/impex.hxx>

using namespace vigra;

template <class T, unsigned int N>
void gaussianSmoothMultiArray(JNIEnv *env, jlongArray shape, jint typeId, jobject sourceData, jobject destData, jdouble sigma )
{
	TinyVector<jlong,N> v;
	env->GetLongArrayRegion( shape, 0, N, &v[0] );
	void* sourcePtr = env->GetDirectBufferAddress(sourceData);
	MultiArrayView<N,T> source( v, reinterpret_cast<T*>(sourcePtr));
	void* destPtr = env->GetDirectBufferAddress(destData);
	MultiArrayView<N,T> dest( v, reinterpret_cast<T*>(destPtr));
	gaussianSmoothMultiArray(source, dest, sigma);
}

template <class T>
void gaussianSmoothMultiArray(JNIEnv *env, jlongArray shape, jint typeId, jobject sourceData, jobject destData, jdouble sigma )
{
	#define F(N) gaussianSmoothMultiArray<T,N>(env, shape, typeId, sourceData, destData, sigma)
	ALLOW_DIMENSIONS(env->GetArrayLength(shape), 1, 2, 3)
    #undef F
}

/*
 * This is the actual JNI call.
 * It uses the ALLOW_TYPES macro to dispatch to gaussianSmoothMultiArray<T>(...) instantiation for the specified types.
 * These instantiations will in turn dispatch to gaussianSmoothMultiArray<T,N>(...) instantiations for the dimensionalities specified there.
 */
JNIEXPORT void JNICALL Java_net_imglib2_vigra_VigraWrapper_gaussianSmoothMultiArray
  (JNIEnv *env, jclass, jlongArray shape, jint typeId, jobject sourceData, jobject destData, jdouble sigma)
{
    using namespace vigra; // to get UInt8 and Int32
	#define F(T) gaussianSmoothMultiArray<T>(env, shape, typeId, sourceData, destData, sigma)
	ALLOW_TYPES(typeId, UInt8, Int32, float)
    #undef F
}

JNIEXPORT void JNICALL Java_net_imglib2_vigra_VigraWrapper_arrayMetadata
  (JNIEnv *env, jclass, jobject sourceData)
{
    jclass clazz = env->GetObjectClass(sourceData);
    jmethodID mid = env->GetMethodID(clazz, "getClass", "()Ljava/lang/Class;");
    if(mid == 0)
    {
        std::cerr << "Method not found\n";
        return;
    }
    jobject arrayClass = env->CallObjectMethod(sourceData, mid);
    jclass clazz2 = env->GetObjectClass(arrayClass);
    jmethodID mid2 = env->GetMethodID(clazz2, "toString", "()Ljava/lang/String;");
    if(mid2 == 0)
    {
        std::cerr << "Method 2 not found\n";
        return;
    }
    
    jstring className = (jstring)env->CallObjectMethod(arrayClass, mid2);
    jboolean isCopy = false;
    std::cerr << env->GetStringUTFChars(className, &isCopy) << "\n#############################\n\n";
}


/*
* New JNI call
*/
/*
 * This is the actual JNI call.
 * It uses the ALLOW_TYPES macro to dispatch to gaussianSmoothMultiArray<T>(...) instantiation for the specified types.
 * These instantiations will in turn dispatch to gaussianSmoothMultiArray<T,N>(...) instantiations for the dimensionalities specified there.
 */
template <class T, unsigned int N>
void discErosion(JNIEnv *env, jobject sourceData, jobject destData, jint radius )
{
	TinyVector<jlong,N> v;
	void* sourcePtr = env->GetDirectBufferAddress(sourceData);
	MultiArrayView<N,T> source( v, reinterpret_cast<T*>(sourcePtr));
	void* destPtr = env->GetDirectBufferAddress(destData);
	MultiArrayView<N,T> dest( v, reinterpret_cast<T*>(destPtr));
	discErosion(source, dest, radius);
}

JNIEXPORT void JNICALL Java_net_imglib2_vigra_VigraWrapper_discErosion
  (JNIEnv *env, jclass, jobject sourceData, jobject destData, jint radius)
{
    using namespace vigra; // to get UInt8 and Int32
	#define F(T) discErosion<T>(env, sourceData, destData, radius)
//	ALLOW_TYPES(typeId, UInt8, Int32, float)
    #undef F
}



/*
*	edgeDetector must be 1 or 2
*/

//template <class T, unsigned int N>
void edgeDetection(JNIEnv *env, const char* src, const char *dst, jint edgeDetector, jdouble scale, jdouble threshold)
{
/*
	std::cout << "THRE" << threshold;
	TinyVector<jlong,N> v;
	env->GetLongArrayRegion( shape, 0, N, &v[0] );
	void* sourcePtr = env->GetDirectBufferAddress(src);
	MultiArrayView<N,T> source( v, reinterpret_cast<T*>(sourcePtr));
	void* destPtr = env->GetDirectBufferAddress(dst);
	MultiArrayView<N,T> dest( v, reinterpret_cast<T*>(destPtr));
	cannyEdgeImage(source, dest, scale, threshold);
	cannyEdgeImage(srcImageRange(source), dest(0), 0.8, 4.0, 1);
        exportImage("/opt/vigra-imglib2/prueba.png", dest);
*/


        ImageImportInfo info(src);
        MultiArray<2, UInt8> in(info.shape());
        importImage(info, in);
        // create output image of appropriate size
        MultiArray<2, UInt8> out(info.shape());      
        // paint output image white
        out = 255;
        if(edgeDetector == 2)
        {
            // call Shen-Castan edge detection algorithm
            // edges will be marked black
            differenceOfExponentialEdgeImage(in, out, scale, threshold, 0);
        }
        else
        {
            // call Canny edge detection algorithm
            // edges will be marked black
            cannyEdgeImage(in, out, scale, threshold, 0);
        }
        exportImage(out, ImageExportInfo(dst));

}

JNIEXPORT void JNICALL Java_net_imglib2_vigra_VigraWrapper_edgeDetection
  (JNIEnv *env, jclass, jstring sourceData, jstring destData,  jint detectionMethod, jdouble scale, jdouble threshold)
{
    using namespace vigra; // to get UInt8 and Int32

	const char* src = env->GetStringUTFChars(sourceData, 0);
	const char* dst = env->GetStringUTFChars(destData, 0);
	
//	#define E(T) 
	edgeDetection(env, src, dst, detectionMethod, scale, threshold);
//	fourierTransform<T>(env, sourceData)
//	ALLOW_TYPES(typeId, UInt8, Int32, float)
//    #undef E
}




