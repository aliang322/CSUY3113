#include "Scene.h"
class menu : public Scene {

public:
	void Initialize(int livesLeft) override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};