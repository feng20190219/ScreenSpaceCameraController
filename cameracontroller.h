#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "sscc_global.h"
#include "cartesian3.h"
#include "matrix4.h"
#include "ray.h"
#include "cartographic.h"
#include "rectangle.h"

class LiScene;
class LiCamera;
class Ellipsoid;
class Globe;
class Tween;
class TweenCollection;
class LiTransform;

/**
 * @brief 相机的相关操作类
 *
 */
class CONTROLLER_EXPORT CameraController : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 默认构造
     * @param parent 父类指针
     *
     */
    CameraController(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     *
     */
    ~CameraController();

    /**
     * @brief 带参构造
     *
     * @param scene 场景
     * @param camera 相机
     * @param tweens 动画合集
     * @param parent 父类指针
     */
    CameraController(LiScene *scene, LiCamera *camera, TweenCollection *tweens, QObject *parent = nullptr);

    /**
     * @brief 改变相机的状态(位置, 偏转角度等)
     *
     * @param destination 目标点
     * @param heading 相机的heading
     * @param pitch 相机的pitch
     * @param roll 相机的roll
     */
    void setView(const Cartesian3 &destination, double heading, double pitch, double roll);

    /**
     * @brief 设置相机的transform (非LiTransform)
     *
     * @param transform 4×4矩阵
     */
    void _setTransform(const Matrix4 &transform);

    /**
     * @brief 拾取
     *
     * @param x 屏幕坐标的x值
     * @param y 屏幕坐标的y值
     * @return Cartesian3 三维场景里的一个点 (世界坐标)
     */
    Cartesian3 pickPoint(double x, double y/*, bool includeTerrainSurface*/);

    /**
     * @brief 在椭球上拾取
     *
     * @param windowPosition 屏幕坐标
     * @param ellipsoid 椭球
     * @param result 按引用传递一个参数
     * @return Cartesian3 三维场景里的一个点 (世界坐标)
     */
    Cartesian3 pickEllipsoid(const Cartesian2 &windowPosition, Ellipsoid *ellipsoid, Cartesian3 &result);

    /**
     * @brief 在椭球上拾取
     *
     * @param windowPosition 屏幕坐标
     * @param ellipsoid 椭球
     * @param result 按引用传递一个参数, 最后变成拾取的结果
     */
    void pickEllipsoid3D(const Cartesian2 &windowPosition, Ellipsoid *ellipsoid, Cartesian3 &result);

    /**
     * @brief 从相机向一个屏幕点(x, y)发射一条射线 (相机拾取)
     *
     * @param x 屏幕坐标的 x
     * @param y 屏幕坐标的 y
     * @return Ray 射线
     */
    Q_INVOKABLE Ray getPickRay(double x, double y);

    /**
     * @brief  相机围绕axis轴旋转
     *
     * @param axis 围绕轴
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotate(const Vector3 &axis, double angle);

    /**
     * @brief 相机围绕自身中心点向上旋转
     *
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotateUp(double angle);

    /**
     * @brief 相机围绕自身中心点向下旋转
     *
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotateDown(double angle);

    /**
     * @brief 相机围绕自身中心点向右旋转
     *
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotateRight(double angle);

    /**
     * @brief 相机围绕自身中心点向左旋转
     *
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void rotateLeft(double angle);

    /**
     * @brief 相机沿着dir方向平移
     *
     * @param dir 平移的方向
     * @param amount 平移量
     */
    Q_INVOKABLE void move(const Vector3 &dir, double amount);

    /**
     * @brief 相机沿着y轴方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveForward(double amount);

    /**
     * @brief 相机沿着y轴反方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveBackward(double amount);

    /**
     * @brief 相机沿着z轴方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveUp(double amount);

    /**
     * @brief 相机沿着z轴反方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveDown(double amount);

    /**
     * @brief 相机沿着x轴方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveRight(double amount);

    /**
     * @brief 相机沿着x轴反方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void moveLeft(double amount);

    /**
     * @brief 相机的每一个方向(x、y、z)分别围绕axis轴旋转
     *
     * @param axis 围绕轴
     * @param angle 旋转角度 (弧度)
     */
    Q_INVOKABLE void look(const Vector3 &axis, double angle);

    /**
     * @brief 相机围绕x轴旋转
     *
     * @param amount 旋转量 (弧度)
     */
    Q_INVOKABLE void lookUp(double amount);

    /**
     * @brief 相机围绕x轴反方向旋转
     *
     * @param amount 旋转量 (弧度)
     */
    Q_INVOKABLE void lookDown(double amount);

    /**
     * @brief 相机围绕z轴旋转
     *
     * @param amount 旋转量 (弧度)
     */
    Q_INVOKABLE void lookRight(double amount);

    /**
     * @brief 相机围绕z轴反方向旋转
     *
     * @param amount 旋转量 (弧度)
     */
    Q_INVOKABLE void lookLeft(double amount);

    /**
     * @brief 相机沿着y轴方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void zoomIn(double amount);

    /**
     * @brief 相机沿着y轴反方向平移
     *
     * @param amount 平移量
     */
    Q_INVOKABLE void zoomOut(double amount);

    /**
     * @brief LiRectangle类型转为Cartesian3类型
     *
     * @param rectangle 需要转换的LiRectangle
     * @return Cartesian3 返回Cartesian3类型
     */
    Cartesian3 getRectangleCameraCoordinates(const LiRectangle &rectangle);

    /**
     * @brief 取消相机的飞行
     *
     */
    void cancelFlight();

    /**
     * @brief 相机飞到目标点 (Vector3类型)
     *
     * @param destination 目标点
     * @param duration 飞行时间 (秒)
     * @param heading 飞到目标点后, 更新相机的heading
     * @param pitch 飞到目标点后, 更新相机的pitch
     * @param roll 飞到目标点后, 更新相机的roll
     */
    Q_INVOKABLE void flyTo(const Vector3 &destination, double duration  = Math::EPSILON20, double heading = 0, double pitch = -90, double roll = 0);

    /**
     * @brief 相机飞到目标点 (Cartographic类型)
     *
     * @param destination 目标点
     * @param duration 飞行时间 (秒)
     * @param heading 飞到目标点后, 更新相机的heading
     * @param pitch 飞到目标点后, 更新相机的pitch
     * @param roll 飞到目标点后, 更新相机的roll
     */
    Q_INVOKABLE void flyTo(const Cartographic &destination, double duration  = Math::EPSILON20, double heading = 0, double pitch = -90, double roll = 0);

    /**
     * @brief 相机飞到目标点 (LiRectangle类型)
     *
     * @param destination 目标点
     * @param duration 飞行时间 (秒)
     * @param heading 飞到目标点后, 更新相机的heading
     * @param pitch 飞到目标点后, 更新相机的pitch
     * @param roll 飞到目标点后, 更新相机的roll
     */
    Q_INVOKABLE void flyTo(const LiRectangle &destination, double duration  = Math::EPSILON20, double heading = 0, double pitch = -90, double roll = 0);

    /**
     * @brief 获取相机更新后的转置矩阵
     *
     * @return Matrix4 4×4矩阵
     */
    Matrix4 invTransform();

    /**
     * @brief 获取相机更新后的世界坐标
     *
     * @return Cartesian3 返回Cartesian3类型
     */
    Cartesian3 positionWC();

    /**
     * @brief 获取相机更新后的y轴方向
     *
     * @return Cartesian3 返回Cartesian3类型
     */
    Cartesian3 directionWC();

    /**
     * @brief 获取相机更新后的z轴方向
     *
     * @return Cartesian3 返回Cartesian3类型
     */
    Cartesian3 upWC();

    /**
     * @brief 获取相机更新后的x轴方向
     *
     * @return Cartesian3 返回Cartesian3类型
     */
    Cartesian3 rightWC();

    /**
     * @brief 获取相机更新后的Cartographic坐标
     *
     * @return Cartographic 返回Cartographic类型
     */
    Cartographic positionCartographic();

    /**
     * @brief 将一个世界坐标点转换到相机坐标系
     *
     * @param cartesian 按引用传递一个参数
     */
    void worldToCameraCoordinates(Cartesian3 &cartesian);

    /**
     * @brief 获取相机的 heading
     *
     * @return double 返回double类型
     */
    double heading();

    /**
     * @brief 获取相机的 pitch
     *
     * @return double 返回double类型
     */
    double pitch();

    /**
     * @brief 获取相机的 roll
     *
     * @return double 返回double类型
     */
    double roll();

    Matrix4 _transform; ///< 相机的矩阵
    Matrix4 _invTransform; ///< 相机的转置矩阵
    Matrix4 _actualTransform; ///< 相机的实际矩阵
    Matrix4 _actualInvTransform; ///< 相机的实际转置矩阵
    bool _transformChanged = false; ///< 相机的矩阵是否发生改变

    Cartesian3 _position; ///< 相机的世界坐标
    Cartesian3 _positionWC; ///< 更新后相机的世界坐标
    Cartographic _positionCartographic; ///< 更新后相机的Cartographic坐标

    Cartesian3 _direction; ///< 相机的y轴方向
    Cartesian3 _directionWC; ///< 更新后相机的y轴方向

    Cartesian3 _up; ///< 相机的z轴方向
    Cartesian3 _upWC; ///< 更新后相机的z轴方向

    Cartesian3 _right; ///< 相机的x轴方向
    Cartesian3 _rightWC; ///< 更新后相机的x轴方向

    Cartesian3 constrainedAxis = Cartesian3(Math::EPSILON20, 0, 0); ///< 相机的默认旋转轴
    double _minimumCollisionTerrainHeight = 15000.0; ///< 测试与地形碰撞前相机必须达到的最小高度, 可由ScreenSpaceCameraController修改

private:
    Ray getPickRayPerspective(double wx, double wy);

    void rotateVertical(double angle);
    void rotateHorizontal(double angle);

    void zoom3D(double amount);

    void updateMembers();

    double getHeading(const Cartesian3 &direction, const Cartesian3 &up);
    double getPitch(const Cartesian3 &direction);
    double getRoll(const Cartesian3 &direction, const Cartesian3 &up, const Cartesian3 &right);

    void setView3D(const Cartesian3 &destination, double heading, double pitch, double roll);

    Cartesian3 rectangleCameraPosition3D(const LiRectangle &rectangle);

    double computeD(const Cartesian3 &direction, const Cartesian3 &upOrRight, const Cartesian3 &corner, double tanThetaOrPhi);

    Cartesian3 multiplyByPoint(const Matrix4 &matrix, const Cartesian3 &cartesian);
    Cartesian3 multiplyByPointAsVector(const Matrix4 &matrix, const Cartesian3 &cartesian);

    LiScene *m_scene;
    Globe *m_globe;
    LiCamera *m_camera;
    LiTransform *m_cameraTrans;
    TweenCollection *m_tweens;
    Tween *_currentFlight = nullptr;
    bool _suspendTerrainAdjustment = false;

    struct CameraRF {
        Cartesian3 direction;
        Cartesian3 right;
        Cartesian3 up;
    };
};

#endif // CAMERACONTROLLER_H
