#pragma once

/// <summary>
/// 유니티 구조를 일부 따라해 보았다.
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

