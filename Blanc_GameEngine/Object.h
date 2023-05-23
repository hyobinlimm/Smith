#pragma once

/// <summary>
/// ����Ƽ ������ �Ϻ� ������ ���Ҵ�.
/// </summary>

class Object
{
public:
	Object() = default;
	virtual ~Object() = default;

public:
	virtual void Awake() abstract;
	virtual void Start() abstract;
	virtual void Update() abstract;
	//virtual void Render() abstract;
};

