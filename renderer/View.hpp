
#pragma once

class View final : public IView
{
public:
	ViewDesc& GetDesc() override;
	const ViewDesc& GetDesc() const override;
private:
	ViewDesc desc;
};
