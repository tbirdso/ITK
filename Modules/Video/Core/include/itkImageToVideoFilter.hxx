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
#ifndef itkImageToVideoFilter_hxx
#define itkImageToVideoFilter_hxx

#include "itkImageToVideoFilter.h"
#include "itkExtractImageFilter.h"

namespace itk
{

//-CONSTRUCTOR PRINT-----------------------------------------------------------

//
// Constructor
//
template <typename TInputImage, typename TOutputVideoStream>
ImageToVideoFilter<TInputImage, TOutputVideoStream>::ImageToVideoFilter()
{
  this->SetNumberOfRequiredInputs(1);
}

template <typename TInputImage, typename TOutputVideoStream>
void
ImageToVideoFilter<TInputImage, TOutputVideoStream>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

//-PUBLIC METHODS--------------------------------------------------------------

//
// SetInput( videoStream )
//
template <typename TInputImage, typename TOutputVideoStream>
void
ImageToVideoFilter<TInputImage, TOutputVideoStream>::SetInput(const TInputImage * image)
{
  // We keep this const_cast because in actuality, we do want to be able to
  // change the requested regions on the input so we need a non-const version
  this->SetInput(0, const_cast<InputImageType *>(image));
}

//
// SetInput( idx, videoStream )
//
template <typename TInputImage, typename TOutputVideoStream>
void
ImageToVideoFilter<TInputImage, TOutputVideoStream>::SetInput(unsigned int idx, const TInputImage * image)
{
  // We keep this const_cast because in actuality, we do want to be able to
  // change the requested regions on the input so we need a non-const version
  this->TemporalProcessObject::SetNthInput(idx, const_cast<InputImageType *>(image));
}

//
// GetInput()
//
template <typename TInputImage, typename TOutputVideoStream>
const TInputImage *
ImageToVideoFilter<TInputImage, TOutputVideoStream>::GetInput() const
{
  if (this->GetNumberOfInputs() < 1)
  {
    return nullptr;
  }
  return static_cast<const InputImageType *>(this->ProcessObject::GetInput(0));
}

//
// GetInput(idx)
//
template <typename TInputImage, typename TOutputVideoStream>
const TInputImage *
ImageToVideoFilter<TInputImage, TOutputVideoStream>::GetInput(unsigned int idx) const
{
  return static_cast<const InputImageType *>(this->ProcessObject::GetInput(idx));
}

//-PROTECTED METHODS-----------------------------------------------------------

//
// GetInput (non-const)
//
template <typename TInputImage, typename TOutputVideoStream>
TInputImage *
ImageToVideoFilter<TInputImage, TOutputVideoStream>::GetInput()
{
  return GetInput(0);
}

//
// GetInput (non-const)
//
template <typename TInputImage, typename TOutputVideoStream>
TInputImage *
ImageToVideoFilter<TInputImage, TOutputVideoStream>::GetInput(unsigned int idx)
{
  return static_cast<InputImageType *>(this->ProcessObject::GetInput(idx));
}

//
// GenerateOutputInformation
//
template <typename TInputImage, typename TOutputVideoStream>
void
ImageToVideoFilter<TInputImage, TOutputVideoStream>::GenerateOutputInformation()
{
  // Get the input
  const InputImageType * input = this->GetInput();

  // Get first input frame's largest possible spatial region
  InputImageRegionType inputRegion = input->GetLargestPossibleRegion();

  // Set temporal frame + duration from user-defined frame axis in input image
  OutputTemporalRegionType outputTemporalRegion;
  outputTemporalRegion.SetFrameStart(inputRegion.GetIndex(m_FrameAxis));
  outputTemporalRegion.SetFrameDuration(inputRegion.GetSize(m_FrameAxis));
  // TODO set real start and real duration
  this->GetOutput()->SetLargestPossibleTemporalRegion(outputTemporalRegion);

  // Set the output spatial region from the input image's largest spatial region,
  // discarding along the user-defined image axis
  OutputFrameSpatialRegionType                 outputSpatialRegion;
  OutputFrameSpatialRegionType::IndexValueType outputIdx = 0;
  for (typename itk::IndexValueType idx = 0; idx < InputImageRegionType::ImageDimension; idx++)
  {
    if (idx != m_FrameAxis)
    {
      itk::SizeValueType  axisSize = inputRegion.GetSize(idx);
      itk::IndexValueType axisStart = inputRegion.GetIndex(idx);
      outputSpatialRegion.SetSize(outputIdx, axisSize);
      outputSpatialRegion.SetIndex(outputIdx, axisStart);
      ++outputIdx;
    }
  }

  // Propagate this spatial region to output frames
  this->GetOutput()->SetAllLargestPossibleSpatialRegions(outputSpatialRegion);
  this->GetOutput()->SetRequestedRegionToLargestPossibleRegion();
}

//
// UpdateOutputInformation
// The default implementation of UpdateOutputInformation attempts to set the
// largest possible output temporal region from the input, but this is best handled
// in GenerateOutputInformation(). Here we override to call the base
// UpdateOutputInformation() implementation in ProcessObject.
//
template <typename TInputImage, typename TOutputVideoStream>
void
ImageToVideoFilter<TInputImage, TOutputVideoStream>::UpdateOutputInformation()
{
  ProcessObject::UpdateOutputInformation();
}

//
// GenerateOutputRequestedRegion
//
template <typename TInputImage, typename TOutputVideoStream>
void
ImageToVideoFilter<TInputImage, TOutputVideoStream>::GenerateOutputRequestedRegion(DataObject * output)
{
  // Override superclass so that we can set output region from input image

  this->GetOutput()->SetRequestedTemporalRegion(this->GetOutput()->GetLargestPossibleTemporalRegion());

  // Go through the requested temporal region and for any frame that doesn't
  // have a requested spatial region, set it to the largest possible
  SizeValueType outFrameStart = this->GetOutput()->GetRequestedTemporalRegion().GetFrameStart();
  SizeValueType outFrameDuration = this->GetOutput()->GetRequestedTemporalRegion().GetFrameDuration();
  for (SizeValueType i = outFrameStart; i < outFrameStart + outFrameDuration; ++i)
  {
    // Get the requested spatial region for this frame
    OutputFrameSpatialRegionType spatialRegion = this->GetOutput()->GetFrameRequestedSpatialRegion(i);

    // Check if the region has 0 size for all dimensions
    bool validRegion = false;
    for (unsigned int j = 0; j < OutputFrameType::ImageDimension; ++j)
    {
      if (spatialRegion.GetSize()[j])
      {
        validRegion = true;
        break;
      }
    }

    // If region has zero size, set it to match the largest possible region
    if (!validRegion)
    {
      this->GetOutput()->SetFrameRequestedSpatialRegion(i, this->GetOutput()->GetFrameLargestPossibleSpatialRegion(i));
    }
  }
}

//
// GenerateInputRequestedRegion
//
template <typename TInputImage, typename TOutputVideoStream>
void
ImageToVideoFilter<TInputImage, TOutputVideoStream>::GenerateInputRequestedRegion()
{
  this->GetInput()->SetRequestedRegion(this->GetInput()->GetLargestPossibleRegion());
}

//
// GenerateData
//
template <typename TInputImage, typename TOutputVideoStream>
void
ImageToVideoFilter<TInputImage, TOutputVideoStream>::GenerateData()
{
  // Rely on SuperClass implementation to allocate output frames
  this->AllocateOutputs();

  // Set each frame in output to an image slice in the input image
  InputImageType *     input = this->GetInput();
  InputImageRegionType inputRegion = input->GetLargestPossibleRegion();

  // Graft input image slices onto output frames
  OutputVideoStreamType * output = this->GetOutput();
  SizeValueType           outputStartFrame = output->GetRequestedTemporalRegion().GetFrameStart();
  SizeValueType           outputDuration = output->GetRequestedTemporalRegion().GetFrameDuration();
  for (auto idx = outputStartFrame; idx < outputStartFrame + outputDuration; idx++)
  {
    InputImageRegionType inputSliceRegion = inputRegion;
    inputSliceRegion.SetSize(m_FrameAxis, 0);
    inputSliceRegion.SetIndex(m_FrameAxis, idx);

    using ExtractFilterType = itk::ExtractImageFilter<InputImageType, OutputFrameType>;
    ExtractFilterType::Pointer extractFilter = ExtractFilterType::New();
    extractFilter->SetDirectionCollapseToSubmatrix();

    extractFilter->SetInput(input);
    extractFilter->SetExtractionRegion(inputSliceRegion);
    extractFilter->Update();

    output->GetFrame(idx)->Graft(extractFilter->GetOutput());
  }
}
} // end namespace itk

#endif // itkImageToVideoFilter_hxx
