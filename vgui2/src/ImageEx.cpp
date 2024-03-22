#include <vgui/ImageEx.h>

ImageEx::ImageEx(SurfaceEx* surface_ex, vgui2::IImage* image) :
    surface_ex_(surface_ex),
    image_(image)
{

}

void ImageEx::Paint()
{
    image_->Paint();
}

void ImageEx::SetPos(int x, int y)
{
    image_->SetPos(x, y);
}

void ImageEx::GetContentSize(int &wide, int &tall)
{
    image_->GetContentSize(wide, tall);
}

void ImageEx::GetSize(int &wide, int &tall)
{
    image_->GetSize(wide, tall);
}

void ImageEx::SetSize(int wide, int tall)
{
    image_->SetSize(wide, tall);
}

void ImageEx::SetColor(Color col)
{
    float alpha_mul = surface_ex_->DrawGetAlphaMultiplier();

    col[3] = (int)((float)col[3] * alpha_mul);

    image_->SetColor(col);
}