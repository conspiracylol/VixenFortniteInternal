#pragma once
#include "Canvas.h"

namespace Framework
{
	namespace ColorPicker
	{
		float ColorAddWhite(float current, float white)
		{
			current += white;

			if (current >= 255.f) return 255.f;

			return current;
		}
	}

	float ColorToFloat(double Color)
	{
		return float(Color / 255.f);
	}

	FLinearColor RGBToFLinearColor(double R, double G, double B)
	{
		return FLinearColor(R / 255.0, G / 255.0, B / 255.0, 1.0);
	}

	class GUI
	{
	public:
		void Initialize()
		{
			this->BackgroundColor = RGBToFLinearColor(0, 0, 0);
			this->MainColor = FLinearColor(1.f, 0.f, 0.f, 1.f);
			this->FrameColor = FLinearColor(0.95f, 0.05f, 0.15f, 1.f);
			this->FrameHoveredColor = RGBToFLinearColor(56, 15, 15);
			this->FramePressedColor = RGBToFLinearColor(56, 15, 15);
			this->TextColor = RGBToFLinearColor(255, 255, 255);

			this->Position = FVector2D(200, 200);
			this->Size = FVector2D(650, 350);
		}

		bool CheckHovered(FVector2D pos, FVector2D Size)
		{
			if (this->Cursor.X > pos.X && this->Cursor.Y > pos.Y)
				if (this->Cursor.X < pos.X + Size.X && this->Cursor.Y < pos.Y + Size.Y)
					return true;

			return false;
		}

		void Combobox(FString Content, bool* Active, int* Value, FString* Arguments, int NumArguments)
		{
			this->Offset.Y += 5;

			FLinearColor RenderColor;
			FVector2D Current = this->Position + this->Offset;
			FVector2D Size = FVector2D(150, 24);
			FVector2D ArrowButtonSize = FVector2D(20, 20); // Adjust the arrow button size as needed

			bool Hovered = this->CheckHovered(Current, Size);
			if (Hovered)
				RenderColor = this->FrameHoveredColor;
			else
				RenderColor = this->FrameColor;

			Wrapper::RectFilled(Current, Size, RenderColor);
			Wrapper::Rect(Current, Size, FLinearColor(0.f, 0.f, 0.f, 1.f), 1.f);

			bool ShowLeftArrow = (*Value > 0);
			if (ShowLeftArrow)
			{
				FVector2D LeftArrowButtonPos = FVector2D(Current.X, Current.Y);
				bool LeftArrowButtonHovered = this->CheckHovered(LeftArrowButtonPos, ArrowButtonSize);
				bool LeftArrowButtonClicked = LeftArrowButtonHovered && this->LeftMouseClicked;

				Wrapper::Text(L"<", FVector2D(LeftArrowButtonPos.X + ArrowButtonSize.X / 4, LeftArrowButtonPos.Y + ArrowButtonSize.Y / 6), this->TextColor, true, false);

				if (LeftArrowButtonClicked)
				{
					*Value -= 1;
				}
			}

			bool ShowRightArrow = (*Value < NumArguments - 1);
			if (ShowRightArrow)
			{
				FVector2D RightArrowButtonPos = FVector2D(Current.X + Size.X - ArrowButtonSize.X, Current.Y);
				bool RightArrowButtonHovered = this->CheckHovered(RightArrowButtonPos, ArrowButtonSize);
				bool RightArrowButtonClicked = RightArrowButtonHovered && this->LeftMouseClicked;

				Wrapper::Text(L">", FVector2D(RightArrowButtonPos.X + ArrowButtonSize.X / 5, RightArrowButtonPos.Y + ArrowButtonSize.Y / 6), this->TextColor, true, false);

				if (RightArrowButtonClicked)
				{
					*Value += 1;
				}
			}

			Wrapper::Text(Arguments[*Value], FVector2D(Current.X + (Size.X / 2) - 2, Current.Y + 3), this->TextColor, true, false);
			Wrapper::Text(Content, FVector2D(Current.X + Size.X + 5, Current.Y + 3), this->TextColor, false, false);

			this->Offset.Y += 26; // 28
		}

		void ColorPicker(FString Content, FLinearColor* Color, bool* Active)
		{
			this->Offset.Y += 6;

			FVector2D Current = this->Position + this->Offset;
			FVector2D Size = FVector2D(9, 9);

			bool Hovered = this->CheckHovered(FVector2D(Current.X - 1, Current.Y - 1), FVector2D(Size.X + 80, Size.Y));
			bool BigHovered = Hovered || (*Active && this->CheckHovered(Current, FVector2D(160, 115)));
			bool Clicked = Hovered && this->LeftMouseClicked;
			bool ClickedSomewhereElse = !BigHovered && this->LeftMouseClicked;

			Wrapper::RectFilled(Current, Size, *Color);
			Wrapper::Rect(Current, Size, FLinearColor(0.f, 0.f, 0.f, 1.f), 1.f);

			Wrapper::Text(Content, FVector2D(Current.X + Size.X + 6, Current.Y - 3), this->TextColor, false, false);

			if (Clicked)
			{
				if (*Active)
					*Active = false;
				else if (!this->TotalOpenPopups)
					*Active = true;
			}

			if (ClickedSomewhereElse && *Active)
				*Active = false;

			if (*Active)
			{
				this->TotalOpenPopups += 1;

				Current = FVector2D(Current.X + Size.X, Current.Y + Size.Y);
				double CurrentXBackup = Current.X;

				//Top -> Bottom (White)
				for (float c = 0; c < 10.f; c++)
				{
					float c_percentage = c / 10.f;

					float c_hx = 200.f * c_percentage;

					//Red -> Green
					for (float i = 0; i < 5.f; i++)
					{
						float percentage = i / 5.f;

						float hx = 255.f * percentage;

						float red = 255.f - hx;
						float green = 255.f - red;
						float blue = 0.f;

						red = ColorPicker::ColorAddWhite(red, c_hx);
						green = ColorPicker::ColorAddWhite(green, c_hx);
						blue = ColorPicker::ColorAddWhite(blue, c_hx);

						FLinearColor converted_color = FLinearColor(red / 255.f, green / 255.f, blue / 255.f, 1.f);

						bool this_color_hovered = this->CheckHovered(Current, FVector2D(11, 11));
						bool this_color_clicked = this->LeftMouseClicked && this_color_hovered;

						if (this_color_clicked)
						{
							*Color = converted_color;
							*Active = false;
						}

						Wrapper::RectFilled(Current, FVector2D(10, 10), converted_color, true);
						Current.X += 10;
					}

					//Green -> Blue
					for (float i = 0; i < 5.f; i++)
					{
						float percentage = i / 5.f;

						float hx = 255.f * percentage;

						float red = 0.f;
						float green = 255.f - hx;
						float blue = 255.f - green;

						red = ColorPicker::ColorAddWhite(red, c_hx);
						green = ColorPicker::ColorAddWhite(green, c_hx);
						blue = ColorPicker::ColorAddWhite(blue, c_hx);

						FLinearColor converted_color = FLinearColor(red / 255.f, green / 255.f, blue / 255.f, 1.f);

						bool this_color_hovered = this->CheckHovered(Current, FVector2D(11, 11));
						bool this_color_clicked = this->LeftMouseClicked && this_color_hovered;

						if (this_color_clicked)
						{
							*Color = converted_color;
							*Active = false;
						}

						Wrapper::RectFilled(Current, FVector2D(10, 10), converted_color, true);
						Current.X += 10;
					}

					//Blue -> Red
					for (float i = 0; i < 5.f; i++)
					{
						float percentage = i / 5.f;

						float hx = 255.f * percentage;

						float green = 0.f;
						float blue = 255.f - hx;
						float red = 255.f - blue;

						red = ColorPicker::ColorAddWhite(red, c_hx);
						green = ColorPicker::ColorAddWhite(green, c_hx);
						blue = ColorPicker::ColorAddWhite(blue, c_hx);

						FLinearColor converted_color = FLinearColor(red / 255.f, green / 255.f, blue / 255.f, 1.f);

						bool this_color_hovered = this->CheckHovered(Current, FVector2D(11, 11));
						bool this_color_clicked = this_color_hovered && this->LeftMouseClicked;

						if (this_color_clicked)
						{
							*Color = converted_color;
							*Active = false;
						}

						Wrapper::RectFilled(Current, FVector2D(10, 10), converted_color, true);
						Current.X += 10;
					}

					Current.X = CurrentXBackup;
					Current.Y += 10;
				}

				Current.X = CurrentXBackup;

				//White -> Black
				for (float i = 0; i < 15.f; i++)
				{
					float percentage = i / 15.f;

					float hx = 255.f * percentage;

					float green = 255.f - hx;
					float blue = 255.f - hx;
					float red = 255.f - hx;

					FLinearColor converted_color = FLinearColor(red / 255.f, green / 255.f, blue / 255.f, 1.f);

					bool this_color_hovered = this->CheckHovered(Current, FVector2D(11, 11));
					bool this_color_clicked = this->LeftMouseClicked && this_color_hovered;

					if (this_color_clicked)
					{
						*Color = converted_color;
						*Active = false;
					}

					Wrapper::RectFilled(Current, FVector2D(10, 10), converted_color, true);
					Current.X += 10;
				}
			}

			this->Offset.Y += 13;
		}

		void Checkbox(FString Content, bool* Option)
		{
			this->Offset.Y += 6;

			FVector2D Current = this->Position + this->Offset;
			FVector2D Size = FVector2D(9, 9);
			FLinearColor RenderColor;

			bool Hovered = this->CheckHovered(FVector2D(Current.X - 1, Current.Y - 1), FVector2D(Size.X + 100, Size.Y + 1));
			bool Clicked = this->LeftMouseClicked && Hovered;

			if (*Option)
				RenderColor = this->MainColor;
			else if (Hovered)
				RenderColor = this->FrameHoveredColor;
			else
				RenderColor = this->FrameColor;

			if (Clicked && !this->TotalOpenPopups) *Option = !*Option;

			Wrapper::RectFilled(Current, Size, RenderColor);
			Wrapper::Rect(Current, Size, FLinearColor(0.f, 0.f, 0.f, 1.f), 1.f);

			Wrapper::Text(Content, FVector2D(Current.X + Size.X + 6, Current.Y - 3), this->TextColor, false, false);

			this->Offset.Y += 13;
		}

		void Slider(FString Content, double* Value, double Min, double Max)
		{
			this->Offset.Y += 6;

			FVector2D Current = this->Position + this->Offset;
			FVector2D Size = FVector2D(120, 10);
			FLinearColor RenderColor;

			bool Hovered = this->CheckHovered(Current, Size);
			bool Down = this->LeftMouseDown && Hovered;

			if (Hovered && Down && !this->TotalOpenPopups)
			{
				*Value = ((this->Cursor.X - Current.X) * ((Max - Min) / Size.X)) + Min;
				if (*Value < Min) *Value = Min;
				if (*Value > Max) *Value = Max;
			}

			if (Down)
				RenderColor = this->FramePressedColor;
			else if (Hovered)
				RenderColor = this->FrameHoveredColor;
			else
				RenderColor = this->FrameColor;

			Wrapper::RectFilled(FVector2D(Current.X, Current.Y + 1), FVector2D(Size.X, Size.Y + 1), RenderColor);
			Wrapper::Rect(FVector2D(Current.X, Current.Y + 1), FVector2D(Size.X, Size.Y + 1), FLinearColor(0.f, 0.f, 0.f, 1.f), 1.f);

			double Percent = Size.X / (Max - Min);
			FVector2D Point = FVector2D(Percent * (*Value - Min), Size.Y);
			Wrapper::RectFilled(FVector2D(Current.X + 1, Current.Y + 2), FVector2D(Point.X, Size.Y), this->MainColor);

			FString ConvertedContent = UKismetStringLibrary::BuildString_Int(Content, FString(L": "), int(*Value), FString(L""), true);
			Wrapper::Text(ConvertedContent, FVector2D(Current.X + Size.X + 6, Current.Y - 3), this->TextColor, false, false, false);

			this->Offset.Y += 14;
		}

		bool MenuButton(FString Content, bool Active)
		{
			FVector2D Current = FVector2D(this->Position.X + this->Offset.X, this->Position.Y + this->Offset.Y);
			FVector2D Size = FVector2D(115, 25);
			FLinearColor RenderColor;

			bool Hovered = this->CheckHovered(Current, Size);
			bool Clicked = this->LeftMouseClicked && Hovered;

			if (Active)
				RenderColor = this->MainColor;
			else if (Hovered)
				RenderColor = this->FrameHoveredColor;
			else
				RenderColor = this->FrameColor;

			Wrapper::RectFilled(Current, Size, RenderColor);
			Wrapper::Rect(Current, Size, FLinearColor(0.f, 0.f, 0.f, 1.f), 1.f);

			Wrapper::Text(Content, FVector2D(Current.X + (Size.X / 2) - 2, Current.Y + 3), this->TextColor, true, false);

			this->Offset.X += 125;

			return Clicked && !this->TotalOpenPopups;
		}

		bool Button(FString Text)
		{
			this->Offset.Y += 5;

			FVector2D Current = this->Position + this->Offset;
			FVector2D Size = FVector2D(120, 22);
			FLinearColor RenderColor;

			bool Hovered = this->CheckHovered(Current, Size);
			bool Clicked = this->LeftMouseClicked && Hovered;
			bool Down = this->LeftMouseDown && Hovered;

			if (Down)
				RenderColor = this->FramePressedColor;
			else if (Hovered)
				RenderColor = this->FrameHoveredColor;
			else
				RenderColor = this->FrameColor;

			Wrapper::RectFilled(Current, FVector2D(Current.X, Size.Y), RenderColor);
			Wrapper::Rect(Current, FVector2D(Current.X, Size.Y), FLinearColor(0.f, 0.f, 0.f, 1.f), 1.f);

			Wrapper::Text(Text, FVector2D(Current.X + (Size.X / 2) - 2, Current.Y + 3), this->TextColor, true, false);

			this->Offset.Y += 28;

			return Clicked && !this->TotalOpenPopups;
		}

		void Text(FString Content)
		{
			FVector2D Current = this->Position + this->Offset;

			Wrapper::Text(Content, Current, this->TextColor, false, false);

			this->Offset.Y += 16;
		}

		void TextColored(FString Content, FLinearColor Color)
		{
			FVector2D Current = this->Position + this->Offset;

			Wrapper::Text(Content, Current, Color, false, false);

			this->Offset.Y += 16;
		}

		FVector2D GetCurrent()
		{
			return this->Position + this->Offset;
		}

		void Space(double X, double Y)
		{
			this->Offset += FVector2D(X, Y);
		}

		void PushX(double value)
		{
			this->Offset.X = value;
		}

		void PushY(double value)
		{
			this->Offset.Y = value;
		}

		void AddOffset(FVector2D ToAddOffset)
		{
			this->Offset.X += ToAddOffset.X;
			this->Offset.Y += ToAddOffset.Y;
		}

		bool CreateMenu(FString Title)
		{
			if (!this->AlreadyInitialized)
			{
				this->Initialize();

				this->AlreadyInitialized = true;
			}

			this->TotalOpenPopups = 0;
			this->Offset = FVector2D(0, 0);

			this->Cursor = Variables::PlayerController->GetMousePosition();
			this->LeftMouseClicked = Variables::PlayerController->WasInputKeyJustPressed(Keys::LeftMouseButton);
			this->LeftMouseDown = Variables::PlayerController->IsInputKeyDown(Keys::LeftMouseButton);
			this->RightMouseClicked = Variables::PlayerController->WasInputKeyJustPressed(Keys::RightMouseButton);
			this->RightMouseDown = Variables::PlayerController->IsInputKeyDown(Keys::RightMouseButton);

			static FVector2D StaticCursor;
			static FVector2D StaticCalculated;

			if (this->CheckHovered(FVector2D(this->Position.X, this->Position.Y - 20.0), FVector2D(this->Size.X, 20.0)) && LeftMouseDown)
			{
				if (!StaticCursor)
				{
					StaticCursor = this->Cursor;
				}

				if (!StaticCalculated)
				{
					StaticCalculated = StaticCursor - this->Position;
				}
			}

			if (StaticCursor && StaticCalculated)
			{
				this->Position = this->Cursor - StaticCalculated;
			}

			if (!this->LeftMouseDown)
			{
				StaticCursor = FVector2D();
				StaticCalculated = FVector2D();
			}

			// 0.0606, 0.0405, 0.0859
			Wrapper::RectFilled(this->Position, this->Size, this->BackgroundColor);
			Wrapper::RectFilled(FVector2D(this->Position.X, this->Position.Y - 20.0), FVector2D(this->Size.X, 20.0), RGBToFLinearColor(51, 5, 5), 1.f); // 0.3693, 0.3392, 0.4441, 1.0
			Wrapper::Text(Title, FVector2D(this->Position.X + this->Size.X / 2, this->Position.Y - 20.0), FLinearColor(1.f, 1.f, 1.f, 1.f), true, false, true);

			return true;
		}

		bool RenderMouse()
		{
			Wrapper::RectFilled(FVector2D(this->Cursor.X - 4, this->Cursor.Y - 4), FVector2D(4, 4), this->TextColor, true);
			return true;
		}
	private:
		bool AlreadyInitialized, LeftMouseClicked, RightMouseClicked, LeftMouseDown, RightMouseDown;
		FVector2D Position, Size, Offset, Cursor, ScreenCenter;
		FLinearColor BackgroundColor, MainColor, FrameColor, FrameHoveredColor, FramePressedColor, TextColor;
		int TotalOpenPopups;
	};

	void DrawMenu()
	{
		static GUI Framework;
		static int MenuTab = int(0);

		Framework.CreateMenu(FString((L"Vixen Internal")));

		Framework.PushX(18);
		Framework.PushY(8);

		if (Framework.MenuButton(FString((L"Combat")), MenuTab == 0)) MenuTab = 0;
		if (Framework.MenuButton(FString((L"Player")), MenuTab == 1)) MenuTab = 1;
		if (Framework.MenuButton(FString((L"Fun")), MenuTab == 2)) MenuTab = 2;

		Framework.PushX(30);
		Framework.PushY(40);

		if (MenuTab == 0)
		{
			Framework.Space(0, 5);
			Framework.Space(-15, 0);
			Framework.Text(FString((L"Aim")));

			Framework.Space(15, 0);

			static bool AimTypeActive = false;
			FString AimTypes[] = { FString((L"Disabled")), FString((L"Memory")), FString((L"Sticky Silent")) };
			Framework.Combobox(FString((L"Type")), &AimTypeActive, &Settings::Aimbot::AimType, AimTypes, sizeof(AimTypes) / sizeof(FString));

			Framework.Checkbox(FString((L"Target line")), &Settings::Aimbot::TargetLine);
			Framework.Checkbox(FString((L"Draw FOV")), &Settings::Aimbot::DrawFOV);
			Framework.Slider(FString((L"FieldOfView")), &Settings::Aimbot::FOV, 0.00, 80.0);
			Framework.Space(0, 5);

			if (Settings::Aimbot::AimType == 1) //AddInput
			{
				Framework.Space(-15, 0);
				Framework.Text(FString((L"Type: Memory")));

				Framework.Space(15, 0);
				Framework.Checkbox(FString(L"Interpolate"), &Settings::Aimbot::Interpolate);
				//Framework.Checkbox(FString(L"Prediction"), &Settings::Aimbot::Prediction);
				Framework.Slider(FString(L"Smoothness"), &Settings::Aimbot::Smoothness, 1.0, 30.0);
			}

			if (Settings::Aimbot::AimType == 2) // Sticky Silent
			{
				Framework.Space(-15, 0);
				Framework.Text(FString((L"Type: Sticky Silent")));
				//Framework.Checkbox(FString(L"Prediction"), &Settings::Aimbot::Prediction);

				Settings::Aimbot::StickySilentAim = true;

				Framework.Space(15, 0);
			}
			else 
			{
				Settings::Aimbot::StickySilentAim = false;
			}
			if (Settings::Aimbot::AimType == 3) // Silent
			{
				Framework.Space(-15, 0);
				Framework.Text(FString((L"Type: Silent")));
				//Framework.Checkbox(FString(L"Prediction"), &Settings::Aimbot::Prediction);

				Settings::Aimbot::SilentAim = true;

				Framework.Space(15, 0);
			}
			else
			{
				Settings::Aimbot::SilentAim = false;
			}

			Framework.Space(0, 5);

			static bool BoneTypeActive = false;
			FString BoneTypes[] = { FString(L"Head"), FString(L"Neck"), FString(L"Chest"), FString(L"Random (2s)") };
			Framework.Combobox(FString(L"Bone"), &BoneTypeActive, &Settings::Aimbot::BoneType, BoneTypes, sizeof(BoneTypes) / sizeof(FString));

			static bool KeyTypeActive = false;
			FString KeyTypes[] = { FString(L"RMB"), FString(L"LMB"), FString(L"Shift"), FString(L"Side Mouse Button 1"), FString(L"Side Mouse Button 2") };
			Framework.Combobox(FString(L"Keybind"), &KeyTypeActive, &Settings::Aimbot::KeyType, KeyTypes, sizeof(KeyTypes) / sizeof(FString));

			Framework.Slider(FString(L"Max Distance"), &Settings::Aimbot::MaxDistance, 0.0, 350.0);

			//switch
			Framework.PushY(41);
			Framework.PushX(420);

			Framework.Space(0, 5);
			Framework.Space(-15, 0);
			Framework.Text(FString((L"Misc")));

			Framework.Space(15, 0);
			//Framework.Checkbox(FString(L"Backtrack"), &Settings::Aimbot::Backtrack);

			Framework.PushX(360);

			Framework.Checkbox(FString(L"Triggerbot"), &Settings::Aimbot::Triggerbot);
			Framework.Checkbox(FString(L"Triggerbot Always Active"), &Settings::Aimbot::TriggerBotAlwaysActive);
			Framework.Combobox(FString(L"TriggerBot Keybind"), &KeyTypeActive, &Settings::Aimbot::TriggerbotKeyType, KeyTypes, sizeof(KeyTypes) / sizeof(FString));
			Framework.Slider(FString(L"Triggerbot Delay"), &Settings::Aimbot::TriggerbotDelay, 5, 500);
			Framework.Slider(FString(L"Triggerbot FOV Size"), &Settings::Aimbot::TriggerBotFOVSize, 5, 500);


			Framework.Checkbox(FString(L"Visible Only"), &Settings::Aimbot::VisableOnly);
			Framework.Checkbox(FString(L"Skip Knocked"), &Settings::Aimbot::SkipKnocked);

			Framework.Space(0, 10);
			Framework.Space(-15, 0);
			Framework.Text(FString(L"Color"));

			Framework.Space(15, 0);

			static bool TargetLineActive = false;
			Framework.ColorPicker(L"Target line", &Settings::Colors::TargetLine, &TargetLineActive);

			static bool FieldOfViewActive = false;
			Framework.ColorPicker(L"FieldOfView", &Settings::Colors::FieldOfView, &FieldOfViewActive);
		}
		else if (MenuTab == 1)
		{
			Framework.Space(0, 5);
			Framework.Space(-15, 0);
			Framework.Text(FString((L"Player")));

			Framework.Space(15, 0);
			static bool BoxTypeActive = false;
			FString BoxTypes[] = { FString((L"Disabled")), FString((L"2D")), FString((L"Corner")), FString((L"Filled Box")) };
			Framework.Combobox(FString((L"Box")), &BoxTypeActive, &Settings::Player::BoxType, BoxTypes, sizeof(BoxTypes) / sizeof(FString));
			Framework.Checkbox(FString((L"Skeleton")), &Settings::Player::Skeleton);
			Framework.Checkbox(FString((L"Show Bots")), &Settings::Player::ShowBots);
			Framework.Checkbox(FString((L"Platform")), &Settings::Player::ShowPlatform);
			Framework.Checkbox(FString((L"Distance")), &Settings::Player::Distance);
			Framework.Checkbox(FString((L"Username")), &Settings::Player::Username);
			Framework.Checkbox(FString((L"Vis Outlines")), &Settings::Player::Outlines);

			Framework.Checkbox(FString((L"Show On Team")), &Settings::Player::ShowTeam);


			//Framework.Checkbox(FString((L"Wireframe")), &Settings::Player::Wireframe);

			Framework.Slider(FString((L"Max Distance")), &Settings::Player::MaxDistance, 0.0, 500.0);

			Framework.PushY(41);
			Framework.PushX(420);

			Framework.Space(0, 5);
			Framework.Space(-15, 0);
			Framework.Text(FString((L"Color")));

			Framework.Space(15, 0);
			static bool TeammateColorActive = false;

			Framework.ColorPicker(FString((L"Teammate")), &Settings::Colors::TeammateColor, &TeammateColorActive);

			static bool BoxVisibleActive = false;
			Framework.ColorPicker(FString((L"Box visible")), &Settings::Colors::BoxVisible, &BoxVisibleActive);

			static bool BoxInVisibleActive = false;
			Framework.ColorPicker(FString((L"Box invisible")), &Settings::Colors::BoxInVisible, &BoxInVisibleActive);

			static bool SkeletonVisibleActive = false;
			Framework.ColorPicker(FString((L"Skeleton visible")), &Settings::Colors::SkeletonVisible, &SkeletonVisibleActive);

			static bool SkeletonInVisibleActive = false;
			Framework.ColorPicker(FString((L"Skeleton invisible")), &Settings::Colors::SkeletonInVisible, &SkeletonInVisibleActive);

			static bool PlayerTextColorActive = false;
			Framework.ColorPicker(FString((L"Player Text Color")), &Settings::Colors::PlayerTextColor, &PlayerTextColorActive);

			static bool ChamColorActive = false;
			Framework.ColorPicker(FString((L"Cham skin color")), &Settings::Colors::SkinColor, &ChamColorActive);

		}
		else if (MenuTab == 2)
		{
			Framework.Space(0, 5);
			Framework.Space(-15, 0);

			Framework.Text(FString((L"Fun")));
			Framework.Checkbox(FString((L"Bullet TP")), &Settings::Exploits::BulletTeleport);
			Framework.Checkbox(FString((L"Bullet TP V2")), &Settings::Exploits::BulletTeleportV3);

			Framework.Checkbox(FString((L"Instant Reload")), &Settings::Exploits::NoReload);
			Framework.Checkbox(FString((L"Spinbot")), &Settings::Exploits::Spinbot);
			Framework.Checkbox(FString((L"Aim While Jumping")), &Settings::Exploits::AimWhileJumping);
			Framework.Checkbox(FString((L"Full Auto Weapons [!!]")), &Settings::Exploits::fullAutoWeapons);
			Framework.Checkbox(FString((L"No Weapon Cooldown [!!!]")), &Settings::Exploits::noWeaponCooldown);
			Framework.Checkbox(FString((L"FOV Changer")), &Settings::Exploits::FOVChanger);
			Framework.Checkbox(FString((L"Bullet Traces")), &Settings::Misc::BulletTraces);

			Framework.PushX(360);
			Framework.PushY(41);

			Framework.Text(FString((L"Other")));

			

			// some: disabled for updates
			Framework.Checkbox(FString((L"No Weapon Cooldown [!!!]")), &Settings::Exploits::noWeaponCooldown);
			//Framework.Checkbox(FString((L"Aim While Reloading")), &Settings::Exploits::reloadWhileAiming);
			Framework.Checkbox(FString((L"Rapid Fire [!!!]")), &Settings::Exploits::RapidFire);
			Framework.Checkbox(FString((L"No Recoil")), &Settings::Exploits::noRecoil);
			Framework.Checkbox(FString((L"Vehicle TP Aura")), &Settings::Exploits::VehicleTpAura);
			Framework.Checkbox(FString((L"Infinite Vehicle Fuel")), &Settings::Exploits::InfiniteFuel);

			Framework.Checkbox(FString((L"Vehicle Fly")), &Settings::Exploits::CarFly);
			
			static bool VehicleFlyTypeActive = false;
			FString VehicleFlyTypes[] = { FString(L"Keys"), FString(L"TP To Marker [F8]") };
			Framework.Combobox(FString(L"Vehicle Fly Type"), &VehicleFlyTypeActive, &Settings::Exploits::VehicleFlyType, VehicleFlyTypes, sizeof(VehicleFlyTypes) / sizeof(FString));			

			Framework.Slider(FString((L"Car Speed")), &Settings::Exploits::CarSpeed, 1.0, 500.0);
			Framework.Slider(FString((L"Spinbot Speed")), &Settings::Exploits::SpinbotSpeed, 1.0, 500.0);
			Framework.Slider(FString((L"FOV Value")), &Settings::Exploits::FOVChangerValue, 60.0, 170.0);
		}

		Framework.RenderMouse();
	}
}