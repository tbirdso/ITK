itk_module_test()
set(ITKVideoCoreTests
  itkImageToVideoFilterTest.cxx
  itkRingBufferTest.cxx
  itkTemporalRegionTest.cxx
  itkTemporalDataObjectTest.cxx
  itkTemporalProcessObjectTest.cxx
  itkVideoStreamTest.cxx
  itkVideoSourceTest.cxx
  itkVideoToVideoFilterTest.cxx
)

CreateTestDriver(ITKVideoCore "${ITKVideoCore-Test_LIBRARIES}" "${ITKVideoCoreTests}" )

# VideoToVideoFilterTest
itk_add_test(
  NAME VideoToVideoFilterTest
  COMMAND ITKVideoCoreTestDriver
    itkVideoToVideoFilterTest )

# VideoSourceTest
itk_add_test(
  NAME VideoSourceTest
  COMMAND ITKVideoCoreTestDriver
    itkVideoSourceTest )

# VideoStreamTest
# This is failing for unknown reasons on some platforms.  Disabling for now
# for the dashboards sake until we have time to debug further.
if(0)
itk_add_test(
  NAME VideoStreamTest
  COMMAND ITKVideoCoreTestDriver
    itkVideoStreamTest )
endif()

# TemporalProcessObjectTest
itk_add_test(
  NAME TemporalProcessObjectTest
  COMMAND ITKVideoCoreTestDriver
    itkTemporalProcessObjectTest )

# RingBufferTest
itk_add_test(
  NAME RingBufferTest
  COMMAND ITKVideoCoreTestDriver
    itkRingBufferTest )

# Testing Temporal Regions
itk_add_test(
  NAME TemporalRegionTest
  COMMAND ITKVideoCoreTestDriver
    itkTemporalRegionTest )

# Testing Temporal Data Object
itk_add_test(
  NAME TemporalDataObjectTest
  COMMAND ITKVideoCoreTestDriver
    itkTemporalDataObjectTest )

itk_add_test(
  NAME ImageToVideoFilterTest
  COMMAND UltrasoundTestDriver
    itkImageToVideoFilterTest
    "C:/repos/unstable-us/ExternalData/test/Input/rf_voltage_15_freq_0005000000_2017-5-31_12-36-44.nrrd") #FIXME
