/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkImageToVideoFilter_h
#define itkImageToVideoFilter_h

#include "itkImage.h"
#include "itkVideoSource.h"
#include "itkVideoStream.h"
#include "itkTemporalRegion.h"
#include "itkMacro.h"

namespace itk
{

/**
 *\class ImageToVideoFilter
 * \brief Base class for filters that use an Image as input and a VideoStream as output
 *
 * ImageToVideoFilter is the base class for all process objects that output
 * VideoStream data and require Image data as input. This class defines
 * the SetInput() method for setting the input to a filter.
 *
 * An implementation of GenerateInputRequestedRegion() is provided here that
 * uses the implementation from TemporalProcessObject to generate input
 * temporal regions and then provides its own mechanism for generating input
 * spatial regions. The default implementation simply takes the requested
 * spatial region from the first frame of output and uses that as the requested
 * region for each of the input frames.
 *
 * \ingroup ITKVideoCore
 * \ingroup Ultrasound
 */
template <typename TInputImage,
          typename TOutputVideoStream =
            itk::VideoStream<itk::Image<typename TInputImage::PixelType, TInputImage::ImageDimension - 1>>>
class ITK_TEMPLATE_EXPORT ImageToVideoFilter : public VideoSource<TOutputVideoStream>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(ImageToVideoFilter);

  /** Standard class type aliases */
  using InputImageType = TInputImage;
  using OutputVideoStreamType = TOutputVideoStream;
  using Self = ImageToVideoFilter<InputImageType, OutputVideoStreamType>;
  using Superclass = VideoSource<OutputVideoStreamType>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using ConstWeakPointer = WeakPointer<const Self>;

  /** Output type alias */
  using OutputFrameType = typename Superclass::OutputFrameType;
  using OutputFrameSpatialRegionType = typename Superclass::OutputFrameSpatialRegionType;
  using OutputTemporalRegionType = typename TOutputVideoStream::TemporalRegionType;

  /** Input type alias */
  using InputImageType = TInputImage;
  using InputImagePointer = typename InputImageType::Pointer;
  using InputImageConstPointer = typename InputImageType::ConstPointer;
  using InputImageRegionType = typename InputImageType::RegionType;
  using InputImagePixelType = typename InputImageType::PixelType;
  using InputImageIndexType = typename InputImageType::IndexType;
  static constexpr unsigned int InputImageDimension = TInputImage::ImageDimension;

  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToVideoFilter, VideoSource);

  /** Set the input VideoStream for this temporal process object */
  using Superclass::SetInput;
  virtual void
  SetInput(const InputImageType * image);

  virtual void
  SetInput(unsigned int idx, const InputImageType * videoStream);

  /** Get the input Image for this temporal process object */
  const InputImageType *
  GetInput() const;

  const InputImageType *
  GetInput(unsigned int idx) const;

  /** Allow the user to specify the axis in the input image that will
   *  correspond to the temporal axis in the output temporal object. */
  itkGetMacro(FrameAxis, typename itk::IndexValueType);
  itkSetMacro(FrameAxis, typename itk::IndexValueType);

protected:
  ImageToVideoFilter();
  ~ImageToVideoFilter() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** Get a non-const version of the input for internal use when setting
   * input's requested regions. This is the only time input should be modified
   */
  InputImageType *
  GetInput();

  InputImageType *
  GetInput(unsigned int idx);

  /** Set up the output video stream via spatial and temporal regions
   * derived from the spatial regions of the input image.
   */
  void
  GenerateOutputInformation() override;

  /** Override the Superclass::UpdateOutputInformation method
   * so that the temporal output region is defined entirely within
   * the GenerateOutputInformation method. */
  void
  UpdateOutputInformation() override;

  void
  GenerateOutputRequestedRegion(DataObject * output);

  /** Override the default implementation of GenerateInputRequestedRegion from
   * VideoSource so that we only get spatial regions from the image input.
   */
  void
  GenerateInputRequestedRegion() override;

  /** Copy pixel data from input image into video frames */
  void
  GenerateData() override;

private:
  typename itk::IndexValueType m_FrameAxis;

}; // end class ImageToVideoFilter

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkImageToVideoFilter.hxx"
#endif

#endif // itkImageToVideoFilter_h
