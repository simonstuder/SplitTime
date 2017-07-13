#include "AdaptiveSelectiveBackgroundLearning.h"

AdaptiveSelectiveBackgroundLearning::AdaptiveSelectiveBackgroundLearning() : firstTime(true), 
alphaLearn(0.05), alphaDetection(0.05), learningFrames(5), counter(0), minVal(0.0), maxVal(1.0),
threshold(15), showOutput(true)
{
  //std::cout << "AdaptiveSelectiveBackgroundLearning()" << std::endl;
}

AdaptiveSelectiveBackgroundLearning::~AdaptiveSelectiveBackgroundLearning()
{
  //std::cout << "~AdaptiveSelectiveBackgroundLearning()" << std::endl;
}

void AdaptiveSelectiveBackgroundLearning::process(const cv::Mat &img_input_, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input_.empty())
    return;

  cv::Mat img_input;
  if (img_input_.channels() == 3)
    cv::cvtColor(img_input_, img_input, CV_BGR2GRAY);
  else
    img_input_.copyTo(img_input);

  //loadConfig();
  learningFrames = 70;
  alphaLearn = 0.01;
  threshold = 25;
  showOutput = false;

  if(firstTime)
    //saveConfig();

  if(img_background.empty())
    img_input.copyTo(img_background);

  cv::Mat img_input_f(img_input.size(), CV_32F);
  img_input.convertTo(img_input_f, CV_32F, 1./255.);

  cv::Mat img_background_f(img_background.size(), CV_32F);
  img_background.convertTo(img_background_f, CV_32F, 1./255.);

  cv::Mat img_diff_f(img_input.size(), CV_32F);
  cv::absdiff(img_input_f, img_background_f, img_diff_f);

  cv::Mat img_foreground(img_input.size(), CV_8U);
  img_diff_f.convertTo(img_foreground, CV_8U, 255.0 / (maxVal - minVal), -minVal);

  cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);
  cv::medianBlur(img_foreground, img_foreground, 3);

  if (learningFrames > 0 && counter <= learningFrames)
  {
    //std::cout << "Adaptive update..." << std::endl;
    // Only Adaptive update of the background model
    img_background_f = alphaLearn * img_input_f + (1 - alphaLearn) * img_background_f;
    counter++;
  }
  else
  {
    //std::cout << "Adaptive and Selective update..." << std::endl;
    int rows = img_input.rows;
    int cols = img_input.cols;

    for (int i = 0; i < rows; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        // Adaptive and Selective update of the background model
        if (img_foreground.at<uchar>(i, j) == 0)
        {
          img_background_f.at<float>(i, j) = alphaDetection * img_input_f.at<float>(i, j) + (1 - alphaDetection) * img_background_f.at<float>(i, j);
        }
      }
    }
  }

  cv::Mat img_new_background(img_input.size(), CV_8U);
  img_background_f.convertTo(img_new_background, CV_8U, 255.0 / (maxVal - minVal), -minVal);
  img_new_background.copyTo(img_background);

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void AdaptiveSelectiveBackgroundLearning::saveConfig()
{
    /*
  CvFileStorage* fs = cvOpenFileStorage("./config/AdaptiveSelectiveBackgroundLearning.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "learningFrames", learningFrames);
  cvWriteReal(fs, "alphaLearn", alphaLearn);
  cvWriteReal(fs, "alphaDetection", alphaDetection);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
  */
}

void AdaptiveSelectiveBackgroundLearning::loadConfig()
{
    /*
  CvFileStorage* fs = cvOpenFileStorage("./config/AdaptiveSelectiveBackgroundLearning.xml", 0, CV_STORAGE_READ);
  
  learningFrames = cvReadIntByName(fs, 0, "learningFrames", 90);
  alphaLearn = cvReadRealByName(fs, 0, "alphaLearn", 0.05);
  alphaDetection = cvReadRealByName(fs, 0, "alphaDetection", 0.05);
  threshold = cvReadIntByName(fs, 0, "threshold", 25);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
    */
}