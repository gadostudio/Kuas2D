#pragma  once

#include "Types.h"
#include "Bitmap.h"
#include "Image.h"
#include "DrawPass.h"
#include "RenderState.h"
#include "Canvas.h"
#include "DrawList.h"
#include "SyncObjects.h"
#include "Surface.h"
#include "RefCount.h"

namespace kuas
{
    class Device : public RefCount
    {
    public:
        virtual ~Device() { }

        /**
            Creates bitmap.
            
            Bitmap is a 2-dimensional array data which can be used to store pixel data. 
            Bitmap is CPU-only storage. Thus, Bitmap memory can be mapped and accessed by CPU.
         */
        virtual Result createBitmap(const BitmapCreateDesc& desc, Bitmap** bitmap) = 0;
        
        /**
            Creates image.
            
            Image is a 2-dimensional array data which can be used for reading or writting pixel data on GPU.
            Image is GPU-only storage. Thus, CPU cannot access its memory directly.
         */
        virtual Result createImage(const ImageCreateDesc& desc, Image** image) = 0;
        
        virtual Result createPathBuffer() = 0;
        
        /**
            Creates draw pass.

            Draw Pass describes the use of Canvas used for painting.
         */
        virtual Result createDrawPass(const DrawPassCreateDesc& desc, DrawPass** drawPass) = 0;
        
        virtual Result createRenderState(DrawPass* drawPass, const RenderStateCreateDesc& desc, RenderState** renderState) = 0;
        
        /**
            Creates canvas.

            Canvas is a collection of Image attachment used for painting.
         */
        virtual Result createCanvas(
            DrawPass* drawPass,
            Image* imageBinding,
            uint32_t width,
            uint32_t height,
            Canvas** canvas) = 0;

        virtual Result createDrawList(DrawList** drawList) = 0;

        virtual Result createSemaphore(Semaphore** semaphore) = 0;

        virtual Result createFence(bool signaled, Fence** fence) = 0;

        virtual Result createSurface(const SurfaceCreateDesc& desc, Surface** surface) = 0;

        virtual void mapBitmap() = 0;

        virtual void unmapBitmap() = 0;

        /**
            Submits wait semaphores.
         */
        virtual Result enqueueWait(
            uint32_t numWaitSemaphores,
            Semaphore* const* waitSemaphores) = 0;

        /**
            Submits draw lists.
         */
        virtual Result enqueueDrawLists(
            uint32_t numDrawLists,
            DrawList* const* drawLists) = 0;

        /**
            Submits signal semaphores and an additional signal fence.
         */
        virtual Result enqueueSignal(
            uint32_t numSignalSemaphores,
            Semaphore* const* signalSemaphores,
            Fence* signalFence = nullptr) = 0;

        virtual Result waitIdle() = 0;

        virtual Result checkBitmapPixelFormatSupport(PixelFormat format, BitmapUsageFlags usage) = 0;
        virtual Result checkImagePixelFormatSupport(PixelFormat format, ImageUsageFlags usage) = 0;

    protected:
        Device() { }
    };
}
