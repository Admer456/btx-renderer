
#pragma once

class Batch final : public IBatch
{
public:
	BatchDesc& GetDesc() override;
	const BatchDesc& GetDesc() const override;
private:
	BatchDesc desc;
};
