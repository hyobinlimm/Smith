#pragma once
#include "Component.h"
class Transform;

/// <summary>
/// 물체의 이동을 담당하는 컴포넌트
/// </summary>
class Movement : public Component
{
public:
    Movement();
    ~Movement();

public:
    void Start() {};
    void Update() {};
    void Awake() {};


    
    /*
    789
    456
    123
    */
    //물체를 dir 방향으로 회전시킨다. dir은 철권의 방향표시법을 사용
    void RotateObject(int dir);
    //물체가 바라보고 있는 방향으로 물체를 이동시킨다.
    void MoveObject();

 

    /*  변수 설정   */
    //물체의 이동속도를 설정한다
    void SetMoveSpeed(float amount);
    //물체의 회전속도를 설정한다.
    void SetRotateSpeed(float amount);

    /*  계산 관련   */
    //쿼터니언을 벡터로 변환
    Vector3 Quat2Vec(Quaternion& quat);
    
private:
	float m_MoveSpeed=0.1;
    float m_RotateSpeed=0.1;

    //라디안 계산용 변수
	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);
	const float Deg2Rad = (PI * 2) / 360;





    typedef int aaaa;
};