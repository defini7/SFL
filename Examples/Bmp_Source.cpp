#define SFL_BMP
#include "SFL.h"

// https://github.com/defini7/defGameEngine
#include "defGameEngine.h"

class Sample : public def::GameEngine
{
public:
	Sample()
	{
		SetTitle("Sample");
	}

protected:
	bool OnUserCreate() override
	{
		sfl::bmp::Load(bmp, "tower.bmp");

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		for (uint32_t x = 0; x < bmp.nWidth; x++)
			for (uint32_t y = 0; y < bmp.nHeight; y++)
			{
				size_t i = y * bmp.nWidth + x;
				Draw(x, y, def::Pixel(bmp.vecPixels[i].r, bmp.vecPixels[i].g, bmp.vecPixels[i].b));
			}

		return true;
	}

private:
	sfl::bmp::Bmp bmp;

};

int main()
{
	Sample demo;

	def::rcode err = demo.Construct(800, 600, 1, 1);

	if (err.ok)
		demo.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
