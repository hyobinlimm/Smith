#pragma once
#include "Component.h"
class Transform;

/// <summary>
/// ��ü�� �̵��� ����ϴ� ������Ʈ
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
    //��ü�� dir �������� ȸ����Ų��. dir�� ö���� ����ǥ�ù��� ���
    void RotateObject(int dir);
    //��ü�� �ٶ󺸰� �ִ� �������� ��ü�� �̵���Ų��.
    void MoveObject();

 

    /*  ���� ����   */
    //��ü�� �̵��ӵ��� �����Ѵ�
    void SetMoveSpeed(float amount);
    //��ü�� ȸ���ӵ��� �����Ѵ�.
    void SetRotateSpeed(float amount);

    /*  ��� ����   */
    //���ʹϾ��� ���ͷ� ��ȯ
    Vector3 Quat2Vec(Quaternion& quat);
    
private:
	float m_MoveSpeed=0.1;
    float m_RotateSpeed=0.1;

    //���� ���� ����
	const float PI = 3.1415926535f;
	const float Rad2Deg = 360 / (PI * 2);
	const float Deg2Rad = (PI * 2) / 360;





    typedef int aaaa;
};