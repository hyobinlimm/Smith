#pragma once
#include "Renderer.h"
#include "HimeRendererStructs.h"

class SceneManager;

/// <summary>
/// �ؽ�Ʈ�� �����ϴ� ������Ʈ 
/// ������Ʈ�鿡�� �ٿ��� �ؽ�Ʈ�� ���� ����
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

public: // get, set �Լ��� 
    shared_ptr<TextData> GetTextData() const { return m_pTextData; }
    
    void SetTexData(std::wstring name, int x, int y, float size, XMFLOAT4 color, bool isDraw, std::wstring text, float value, float value2, float value3);

    bool GetOnEnabled() const { return m_OnEnabled; }
    void SetOnEnabled(bool val) { m_OnEnabled = val; }

private:
    shared_ptr<SceneManager> m_SceneManager;

    shared_ptr<TextData> m_pTextData;
    
    bool m_OnEnabled;
};

