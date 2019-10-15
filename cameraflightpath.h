#ifndef CAMERAFLIGHTPATH_H
#define CAMERAFLIGHTPATH_H

#include "cartesian3.h"
#include "screenspaceeventutils.h"

class Tween;
class LiCamera;
class CameraController;
class ScreenSpaceCameraController;

/**
 * @brief  相机的飞行路径
 *
 */
class CameraFlightPath
{
public:
    CameraFlightPath();

    /**
     * @brief 创建Tween对象, 使用插值函数对对象的两个属性进行插值运算 (静态函数)
     *
     * @param camera 相机
     * @param controller 相机控制类
     * @param newOptions 结构体, 包含相机的位置, 偏转角度等信息
     * @return Tween 返回Tween对象指针
     */
    static Tween *createTween(LiCamera *camera, CameraController *controller, const CameraNewOptions &newOptions);

private:
    static TweenAction wrapCallback(ScreenSpaceCameraController *controller, const TweenAction &action);
    static TweenAction1 createUpdate3D(LiCamera *camera, CameraController *controller, double duration, const Cartesian3 &destination, double heading, double pitch, double roll);
    static double adjustAngleForLERP(double startAngle, double endAngle);
    static TweenAction1Double createHeightFunction(LiCamera *camera, const Cartesian3 &destination, double startHeight, double endHeight);
    static double getAltitude(LiCamera *camera, double dx, double dy);
};

#endif // CAMERAFLIGHTPATH_H
