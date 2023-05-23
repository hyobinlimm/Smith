#pragma once
#include "Renderer.h"
#include "HimeRendererStructs.h"

class SceneManager;

/// <summary>
/// 텍스트를 랜더하는 컴포넌트 
/// 오브젝트들에게 붙여서 텍스트를 띄우는 역할
/// </summary>

class TextRenderer : public Renderer
{
public:
    TextRenderer();
    ~TextRenderer();
    
public:
    void Awake();
    void Start();
    void Update();
    void Render();

public: // get, set 함수들 
    shared_ptr<TextData> GetTextData() const { return m_pTextData; }
    
    void SetTexData(std::wstring name, int x, int y, float size, XMFLOAT4 color, bool isDraw, std::wstring text, float value, float value2, float value3);

    bool GetOnEnabled() const { return m_OnEnabled; }
    void SetOnEnabled(bool val) { m_OnEnabled = val; }

private:
    shared_ptr<SceneManager> m_SceneManager;

    shared_ptr<TextData> m_pTextData;
    
    bool m_OnEnabled;
};

