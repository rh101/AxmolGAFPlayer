#include "GAFPrecompiled.h"
#include "GAFShaderManager.h"
#include "GAFShaders.h"

using std::string;
USING_NS_AX;

NS_GAF_BEGIN

    bool GAFShaderManager::s_initialized = false;

    const std::string_view GAFShaderManager::s_fragmentShaders[] =
    {
        GaussianBlurFragmentShader_fs,                  // GaussBlur
        GlowFragmentShader_fs,                          // Glow
        pcShader_PositionTextureAlpha_frag_fs,          // Alpha
    };

    ax::Program* GAFShaderManager::s_programs[] = {nullptr};

    const char* const GAFShaderManager::s_uniformNames[] = 
    {
        "colorTransformMult",       // ColorTransformMult
        "colorTransformOffsets",    // ColorTransformOffset
        "colorMatrix",              // ColorMatrixBody
        "colorMatrix2",             // ColorMatrixAppendix

        "u_step",                   // BlurTexelOffset

        "u_step",                   // GlowTexelOffset
        "u_glowColor",              // GlowColor
        "u_strength",               // Strength
    };

    //GLint GAFShaderManager::s_uniformLocations[] =
    //{
    //    -1
    //};

    void GAFShaderManager::renderRecreate(EventCustom*)
    {
        Initialize(true);
        AXLOGD("RENDER recreated");
    }

    void GAFShaderManager::Initialize(bool force /*= false*/)
    {
        if (!s_initialized)
        {
            const std::string eventName = EVENT_RENDERER_RECREATED;
            Director::getInstance()->getEventDispatcher()->addCustomEventListener(eventName, GAFShaderManager::renderRecreate);
        }

        bool skip = !force && s_initialized;
        bool reinit = force && s_initialized;

        if (!skip)
        {
            // Alpha
            {
                auto fragmentShader = getShader(EFragmentShader::Alpha);
                Program* program = nullptr;
                if (reinit)
                {
                    program = s_programs[EPrograms::Alpha];
                    //program->reset();
                    //AXASSERT(program->initWithByteArrays(ax::ccPositionTextureColor_vert, fragmentShader), "`Alpha` shader init error");
                    //AXASSERT(program->link(), "`Alpha` shader linking error");
                    //program->updateUniforms();
                }
                else
                {
                    program = ProgramManager::getInstance()->loadProgram(positionTextureColor_vert, fragmentShader);
                    AX_SAFE_RELEASE(s_programs[EPrograms::Alpha]);
                    s_programs[EPrograms::Alpha] = program;
                    AX_SAFE_RETAIN(s_programs[EPrograms::Alpha]);
                }
                AXASSERT(program, "`Alpha` shader not loaded.");

                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_POSITION, ax::GLProgram::VERTEX_ATTRIB_POSITION);
                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_COLOR, ax::GLProgram::VERTEX_ATTRIB_COLOR);
                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_TEX_COORD, ax::GLProgram::VERTEX_ATTRIB_TEX_COORDS);
                //s_uniformLocations[EUniforms::ColorTransformMult] =     glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::ColorTransformMult]);
                //s_uniformLocations[EUniforms::ColorTransformOffset] =   glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::ColorTransformOffset]);
                //s_uniformLocations[EUniforms::ColorMatrixBody] =        glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::ColorMatrixBody]);
                //s_uniformLocations[EUniforms::ColorMatrixAppendix] =    glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::ColorMatrixAppendix]);
            }
            
            // Blur
            {
                auto fs = getShader(EFragmentShader::GaussBlur);
                Program* program = nullptr;
                if (reinit)
                {
                    program = s_programs[EPrograms::Blur];
                    //program->reset();
                    //AXASSERT(program->initWithByteArrays(ax::ccPositionTextureColor_vert, fs), "`Blur` shader init error");
                    //AXASSERT(program->link(), "`Blur` shader linking error");
                    //program->updateUniforms();
                }
                else
                {
                    program = ProgramManager::getInstance()->loadProgram(positionTextureColor_vert, fs);
                    AX_SAFE_RELEASE(s_programs[EPrograms::Blur]);
                    s_programs[EPrograms::Blur] = program;
                    AX_SAFE_RETAIN(s_programs[EPrograms::Blur]);
                }

                AXASSERT(program, "`Blur` shader not loaded.");

                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_POSITION, ax::GLProgram::VERTEX_ATTRIB_POSITION);
                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_COLOR, ax::GLProgram::VERTEX_ATTRIB_COLOR);
                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_TEX_COORD, ax::GLProgram::VERTEX_ATTRIB_TEX_COORDS);
                //s_uniformLocations[EUniforms::BlurTexelOffset] = glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::BlurTexelOffset]);
            }

            // Glow
            {
                auto fs = GAFShaderManager::getShader(GAFShaderManager::EFragmentShader::Glow);
                Program* program = nullptr;
                if (reinit)
                {
                    program = s_programs[EPrograms::Glow];
                    //program->reset();
                    //AXASSERT(program->initWithByteArrays(ax::ccPositionTextureColor_vert, fs), "`Glow` shader init error");
                    //AXASSERT(program->link(), "`Glow` shader linking error");
                    //program->updateUniforms();
                }
                else
                {
                    program = ProgramManager::getInstance()->loadProgram(positionTextureColor_vert, fs);
                    AX_SAFE_RELEASE(s_programs[EPrograms::Glow]);
                    s_programs[EPrograms::Glow] = program;
                    AX_SAFE_RETAIN(s_programs[EPrograms::Glow]);
                }

                AXASSERT(program, "`Glow` shader not loaded.");

                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_POSITION, ax::GLProgram::VERTEX_ATTRIB_POSITION);
                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_COLOR, ax::GLProgram::VERTEX_ATTRIB_COLOR);
                //program->bindAttribLocation(ax::GLProgram::ATTRIBUTE_NAME_TEX_COORD, ax::GLProgram::VERTEX_ATTRIB_TEX_COORDS);
                //s_uniformLocations[EUniforms::GlowTexelOffset] = glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::GlowTexelOffset]);
                //s_uniformLocations[EUniforms::GlowColor] = glGetUniformLocation(program->getProgram(), s_uniformNames[EUniforms::GlowColor]);
            }
            s_initialized = true;
        }
    }
NS_GAF_END
