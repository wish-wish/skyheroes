
#include "ObjectiveChipmunk.h"
#include "ChipmunkAutoGeometry.h"

///Generic sampler used with bitmap data.
///Currently limited to 8 bit per component data.
///Bitmap samplers currently provide no filtering, but could be easily extended to do so.
class ChipmunkBitmapSampler :public ChipmunkAbstractSampler {
private:
	const uint8_t *_pixels;
public:
	/// Width of the bitmap in pixels.
	int width;

	/// Height of the bitmap in pixels.
	int height;

	/// stride
	int stride;

	///flip
	bool flip;

	/// Bytes per pixel of the bitmap. (ex: RGBA8888 would be 4)
	int bytesPerPixel;

	/// Zero-based ndex of the component to sample. (ex: alpha of RGBA would be 3)
	int component;

	///borderValue
	cpFloat borderValue;

	/// NSData object holding the pixel data.
	void *pixelData;

	/// Rect that the image maps to.
	/// Defaults to (0.5, 0.5, width - 0.5, height - 0.5) so that pixel centers will be cleanly sampled.
	cpBB outputRect;


	///Init a sampler from bitmap data.
	///Stride refers to the length of a row of pixels in bytes. (Generally just w*h*bytesPerPixel unless there is padding)
	///Image must use one byte per component, but can have any number of components.
	///@c component refers to the 0-based index of the component to sample. (i.e. 3 would sample the alpha in an RGBA bitmap)
	///@c flip allows you to flip the image vertically to match how it migh be drawn.
	///@c pixelData can be either a NSData or NSMutableData (i.e. for deformable terrain) that contains the bitmap data.
	ChipmunkBitmapSampler* initWith(int width,int height,int stride,int bytesPerPixel,int component,bool flip,void *pixelData,int datalen);

	/// Set the border of the bitmap to repeat the edge pixels.
	void setBorderRepeat();

	/// Set the border of the bitmap to be a specific value.
	void setBorderValue(cpFloat borderValue);

	/// March the entire image.
	ChipmunkPolylineSet *marchAllWithBorder(bool bordered,bool hard);
};



/// Sampler built on top of a CGBitmapContext to allow deformable geometry.
/// Very efficient when paired with a ChipmunkTileCache.
class ChipmunkCGContextSampler :public ChipmunkBitmapSampler {
public:
	/// CGBitmapContext for this sampler.
	Image* context;

	/// NSMutableData object holding the pixel data.
	void *pixelData;

	/// Initialize a context based sampler. Must provide options for a valid context.
	/// Find out more here in the Quartz 2D Programming Guide.
	ChipmunkCGContextSampler* initWith(unsigned long width,unsigned long height,CGColorSpaceRef colorSpace,CGBitmapInfo bitmapInfo,int component);
};



/// A CGBitmapContext sampler initialized with an CGImage.
class ChipmunkImageSampler :public ChipmunkCGContextSampler
{
public:
	/// Initialize an image sampler of a certain size with a CGImage.
	/// If isMask is TRUE, the image will be loaded as a black and white image, if FALSE only the image alpha will be loaded.
	ChipmunkImageSampler* initWith(Image* image,bool isMask,int width,int height);

	/// Initialize an image sampler with an image file.
	/// If isMask is TRUE, the image will be loaded as a black and white image, if FALSE only the image alpha will be loaded.
	ChipmunkImageSampler* initWithImageFile(std::string *url,bool isMask);

	/// Return an autoreleased image sampler initialized with an image file.
	/// If isMask is TRUE, the image will be loaded as a black and white image, if FALSE only the image alpha will be loaded.
	static ChipmunkImageSampler *samplerWithImageFile(std::string url,bool isMask);
};