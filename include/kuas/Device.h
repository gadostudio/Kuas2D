#pragma  once

#include "Types.h"
#include "Bitmap.h"
#include "Image.h"
#include "DrawPass.h"
#include "RenderState.h"
#include "RenderTarget.h"
#include "DrawList.h"
#include "SyncObjects.h"
#include "RefCount.h"

namespace kuas
{
    class Device : public RefCount
    {
    public:
        virtual ~Device() { }

        /**
            Creates bitmap.
            
            Bitmap is a 2-dimensional array data which can be used for reading and sending pixel data to/from GPU. 
            Bitmap is CPU-only storage.
          
         */
        virtual Result createBitmap(const BitmapCreateDesc& desc, Bitmap** bitmap) = 0;
        
        /**
            Creates image.
            
            Image is a 2-dimensional array data which can be used for reading and writting pixel data on GPU.
            Image is GPU-only storage. CPU cannot read and write pixel data directly through Image,
            CPU must use a bitmap-image copy command in order to read and write into an Image.
         */
        virtual Result createImage(const ImageCreateDesc& desc, Image** image) = 0;
        
        virtual Result createPathBuffer() = 0;
        
        virtual Result createDrawPass(const DrawPassCreateDesc& desc, DrawPass** drawPass) = 0;
        
        virtual Result createRenderState(DrawPass* drawPass, const RenderStateCreateDesc& desc, RenderState** renderState) = 0;
        
        virtual Result createRenderTarget(
            DrawPass* drawPass,
            Image* imageBinding,
            uint32_t width,
            uint32_t height,
            RenderTarget** renderTarget) = 0;

        virtual Result createDrawList(DrawList** drawList) = 0;

        virtual Result createSemaphore(Semaphore** semaphore) = 0;

        virtual Result createFence(bool signaled, Fence** fence) = 0;

        virtual void mapBitmap() = 0;

        virtual void unmapBitmap() = 0;

        /**
            Submits draw lists to the GPU.
         */
        virtual Result enqueueWait(
            uint32_t numWaitSemaphore,
            Semaphore* const* waitSemaphores) = 0;

        virtual Result enqueueDrawLists(
            uint32_t numDrawLists,
            DrawList* const* drawLists) = 0;

        virtual Result enqueueSignal(
            uint32_t numSignalSemaphore,
            Semaphore* const* signalSemaphores,
            Fence* signalFence = nullptr) = 0;

        virtual Result checkBitmapPixelFormatSupport(PixelFormat format, BitmapUsageFlags usage) = 0;
        virtual Result checkImagePixelFormatSupport(PixelFormat format, ImageUsageFlags usage) = 0;

    protected:
        Device() { }
    };
}
