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

#include "itkImageToVideoFilter.h"

#include <string>

#include "itkImage.h"
#include "itkVideoStream.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"

int
itkImageToVideoFilterTest(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage outputImage";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }

  using PixelType = float;
  const unsigned int Dimension = 3;

  using ImageType = itk::Image<PixelType, Dimension>;

  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  using VideoFilterType = itk::ImageToVideoFilter<ImageType>;
  using VideoType = typename VideoFilterType::OutputVideoStreamType;
  VideoFilterType::Pointer videoFilter = VideoFilterType::New();
  videoFilter->SetInput(reader->GetOutput());
  itk::IndexValueType frameAxis = 0;
  videoFilter->SetFrameAxis(frameAxis);

  ITK_TRY_EXPECT_NO_EXCEPTION(videoFilter->Update());

  auto videoOutput = videoFilter->GetOutput();
  auto imageRegion = reader->GetOutput()->GetLargestPossibleRegion();

  // Verify start frame and frame duration in output match size of designated temporal axis in input
  ITK_TEST_EXPECT_EQUAL(videoOutput->GetLargestPossibleTemporalRegion().GetFrameStart(),
                        reader->GetOutput()->GetLargestPossibleRegion().GetIndex(frameAxis));
  ITK_TEST_EXPECT_EQUAL(videoOutput->GetLargestPossibleTemporalRegion().GetFrameDuration(),
                        reader->GetOutput()->GetLargestPossibleRegion().GetSize(frameAxis));

  // Verify spatial dimensions in output frames match size of non-temporal axes in input
  for (itk::IndexValueType idx = 1; idx < ImageType::ImageDimension; idx++)
  {
    ITK_TEST_EXPECT_EQUAL(videoOutput->GetFrame(0)->GetLargestPossibleRegion().GetSize(idx - 1),
                          reader->GetOutput()->GetLargestPossibleRegion().GetSize(idx));
  }

  // Verify pixel data matches
  ITK_TEST_EXPECT_EQUAL(videoOutput->GetFrame(0)->GetPixel({ 0, 0 }), reader->GetOutput()->GetPixel({ 0, 0, 0 }));

  // Write out first frame for visual verification
  using WriterType = itk::ImageFileWriter<VideoType::FrameType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(videoOutput->GetFrame(0));
  writer->Update();

  return EXIT_SUCCESS;
}
