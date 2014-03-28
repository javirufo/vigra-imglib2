import ij.ImageJ;
import io.scif.SCIFIO;
import io.scif.img.ImgIOException;
import io.scif.img.ImgOpener;
import net.imglib2.img.Img;
import net.imglib2.img.array.ArrayImg;
import net.imglib2.img.array.ArrayImgBufferFactory;
import net.imglib2.img.display.imagej.ImageJFunctions;
import net.imglib2.type.NativeType;
import net.imglib2.type.numeric.RealType;
import net.imglib2.type.numeric.real.FloatType;
import net.imglib2.vigra.VigraWrapper;

public class Example2
{
	//JAVI
	public static <T extends NativeType<T> & RealType< T > > void smoothWithVigra( final T type ) throws ImgIOException
	{
		final String fn = "ghouse.png";
		final SCIFIO scifio = new SCIFIO();
		final ImgOpener opener = new ImgOpener( scifio.getContext() );
		final ArrayImgBufferFactory< T > imgFactory = new ArrayImgBufferFactory< T >();
		final Img< T > source = opener.openImg( fn, imgFactory, type ).getImg();
//		final Img< T > dest = imgFactory.create( source, type );

		VigraWrapper.arrayMetadata( ( ArrayImg ) source);
//New JNI Functions
		final String dn = "ghouse2.png";
		VigraWrapper.jedgeDetection(fn, dn, 1, 2, 1.3);
//		VigraWrapper.edgeDetection(( ArrayImg )source, ( ArrayImg ) dest, 1, 2, 1.3);
//		VigraWrapper.discErosion(( ArrayImg ) source, ( ArrayImg ) dest, 1 );
		final Img< T > dest = opener.openImg( dn, imgFactory, type ).getImg();
		new ImageJ();
		ImageJFunctions.show( source );
		ImageJFunctions.show( dest );

		scifio.getContext().dispose();
	}

	public static void main( final String... args ) throws Exception
	{
//		smoothWithVigra( new UnsignedByteType() ); // doesn't work currently due to a scifio limitation
//		smoothWithVigra( new IntType() );
		smoothWithVigra( new FloatType() );
	}
}
