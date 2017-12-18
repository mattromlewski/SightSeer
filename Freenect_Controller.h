#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <libfreenect/libfreenect.hpp>
#include "Frame_Mutex.h"


using namespace std;
using namespace cv;

class Freenect_Controller: public Freenect::FreenectDevice
{
public:
    Freenect_Controller(freenect_context* _pContext, int _iIndex)
                        // Parent class constructor, <init list>
                        :Freenect::FreenectDevice(_pContext, _iIndex),
                        m_uiBuffer_depth(FREENECT_DEPTH_11BIT), m_uiBuffer_rgb(FREENECT_VIDEO_RGB), m_uiGamma(2048),
                        m_depthMat(Size(640,480),CV_16UC1), m_rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
                        m_ownMat(Size(640,480),CV_8UC3,Scalar(0)),
                        m_bNewDepthFrame(false), m_bNewRgbFrame(false)
    {
        for( unsigned int i = 0 ; i < 2048 ; i++) {
				float v = i/2048.0;
				v = std::pow(v, 3)* 6;
				m_uiGamma[i] = v*6*256;
        }
    }
    void VideoCallback(void* _pRgb, uint32_t _uiTimestamp){
        std::cout << "RGB callback" << std::endl;
        m_rgb_mutex.lock();
        uint8_t* pRgb = static_cast<uint8_t*>(_pRgb);
        m_rgbMat.data = pRgb;
        m_bNewRgbFrame = true;
        m_rgb_mutex.unlock();
    }

    // Do not call directly even in child
    void DepthCallback(void* _pDepth, uint32_t _uitimestamp) {
        std::cout << "Depth callback" << std::endl;
        m_depth_mutex.lock();
        uint16_t* pDepth = static_cast<uint16_t*>(_pDepth);
        m_depthMat.data = (uchar*) pDepth;
        m_bNewDepthFrame = true;
        m_depth_mutex.unlock();
    }

    bool getVideo(Mat& _outputMatAdr) {
        m_rgb_mutex.lock();
        if(m_bNewRgbFrame) {
            cv::cvtColor(m_rgbMat, _outputMatAdr, CV_RGB2BGR);
            m_bNewRgbFrame = false;
            m_rgb_mutex.unlock();
            return true;
        } else {
            m_rgb_mutex.unlock();
            return false;
        }
    }

    bool getDepth(Mat& _outputMatAdr) {
            m_depth_mutex.lock();
            if(m_bNewDepthFrame) {
                m_depthMat.copyTo(_outputMatAdr);
                m_bNewDepthFrame = false;
                m_depth_mutex.unlock();
                return true;
            } else {
                m_depth_mutex.unlock();
                return false;
            }
        }

private:
    std::vector<uint8_t> m_uiBuffer_depth;
    std::vector<uint8_t> m_uiBuffer_rgb;
    std::vector<uint16_t> m_uiGamma;

    // OpenCV image types
    cv::Mat m_depthMat;
    cv::Mat m_rgbMat;
    cv::Mat m_ownMat;

    // Thread locking/unlocking objects
    Frame_Mutex m_rgb_mutex;
    Frame_Mutex m_depth_mutex;

    // Image acq. flags
    bool m_bNewRgbFrame;
    bool m_bNewDepthFrame;


};
