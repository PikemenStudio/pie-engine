//
// Created by FullHat on 09/06/2024.
//

#include "UI.hpp"

#include "imgui.h"
#include </Users/fullhat/Documents/GitHub/pie-engine/vcpkg/buildtrees/imgui/src/76b6bb5605-ca10468178.clean/backends/imgui_impl_glfw.h>
#include </Users/fullhat/Documents/GitHub/pie-engine/vcpkg/buildtrees/imgui/src/76b6bb5605-ca10468178.clean/backends/imgui_impl_vulkan.h>

void UI::draw(uint32_t width, uint32_t height) {
  UI::width = width;
  UI::height = height;

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  drawList();
  if (CurrentObjectName != "") {
    drawObjectModification();
  }

  //  bool ShowDialog = true;
  //
  //  char textureName[128] = "Texture";
  //  char texturePath[256] = "";
  //  bool showWindow = true;
  //  ImGui::Begin("Texture Input", &showWindow);
  //
  //  // Input field for texture name
  //  ImGui::InputText("Texture Name", textureName, IM_ARRAYSIZE(textureName));
  //
  //  // Input field for texture path
  //  ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));
  //
  //  // Buttons for confirm and cancel
  //  if (ImGui::Button("Confirm")) {
  //    // Handle confirm action (e.g., save texture information)
  //    // You can use textureName and texturePath here
  //    showWindow = false; // Close the window after confirming
  //  }
  //  ImGui::SameLine();
  //  if (ImGui::Button("Cancel")) {
  //    // Handle cancel action
  //    showWindow = false; // Close the window
  //  }
  //
  //  ImGui::End();

  //  bool showDeleteWindow = true;
  //  ImGui::Begin("Delete Texture", &showDeleteWindow);
  //
  //  // Dropdown list for selecting texture
  //  if (ImGui::BeginCombo("Select Texture", "A")) {
  //    if (ImGui::Selectable("Texture1", true)) {
  //    }
  //    ImGui::SetItemDefaultFocus();
  //    if (ImGui::Selectable("Texture2", true)) {
  //    }
  //    ImGui::SetItemDefaultFocus();
  //    ImGui::EndCombo();
  //  }
  //
  //  // Buttons for delete and cancel
  //  if (ImGui::Button("Delete")) {
  //  }
  //  ImGui::SameLine();
  //  if (ImGui::Button("Cancel")) {
  //    // Handle cancel action
  //    showDeleteWindow = false; // Close the window
  //  }
  //  ImGui::End();

  ImGui::Render();
}
void UI::drawList() {
  ImGui::SetNextWindowSize(ImVec2(350, height - 300));
  ImGui::SetNextWindowPos(ImVec2(0, 0)); // Set the position of the new window

  bool ShowDialog = true;
  ImGui::Begin("Objects", &ShowDialog, ImGuiWindowFlags_MenuBar);

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Scene")) {
      if (ImGui::MenuItem("Open", "Ctrl+O")) {
      }
      if (ImGui::MenuItem("Save", "Ctrl+S")) {
      }
      if (ImGui::MenuItem("Delete", "Ctrl+D")) {
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
      }
      if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  if (ImGui::TreeNode("Objects")) {
    for (const auto &ObjectName : Objects) {
      if (ImGui::Selectable(ObjectName.c_str(),
                            ObjectName == CurrentObjectName)) {
        CurrentObjectName = ObjectName;
        CurrentObjectUpdated();
      }
    }
    ImGui::TreePop();
  }
  ImGui::End();
}
void UI::drawObjectModification() {
  ImGui::SetNextWindowSize(ImVec2(350, 300));
  ImGui::SetNextWindowPos(
      ImVec2(0, height - 300)); // Set the position of the new window
  ImGui::Begin("Object Modification", nullptr, ImGuiWindowFlags_NoResize);

  char objectName[128] = "Object Name";
  float rotation[3] = {
      ObjectModification.Rotation.x,
      ObjectModification.Rotation.y,
      ObjectModification.Rotation.z,
  };

  // Set current object modification
  float translation[3] = {
      ObjectModification.Position.x,
      ObjectModification.Position.y,
      ObjectModification.Position.z,
  };
  float scale[3] = {
      ObjectModification.Scale.x,
      ObjectModification.Scale.y,
      ObjectModification.Scale.z,
  };
  std::vector<std::string> textures = {"BlueMiddleSoft", "Red", "Texture3"};
  int currentTexture = 0;

  // Input field for object name
  ImGui::InputText("Object Name", objectName, IM_ARRAYSIZE(objectName));

  // Input field for rotation
  ImGui::InputFloat3("Rotation", rotation);
  ObjectModification.Rotation =
      glm::vec3(rotation[0], rotation[1], rotation[2]);

  // Input field for translation
  ImGui::InputFloat3("Translation", translation);
  ObjectModification.Position =
      glm::vec3(translation[0], translation[1], translation[2]);

  // Input field for scaling
  ImGui::InputFloat3("Scale", scale);
  ObjectModification.Scale = glm::vec3(scale[0], scale[1], scale[2]);

  // Dropdown list for selecting texture
  if (ImGui::BeginCombo("Texture", textures[currentTexture].c_str())) {
    for (int n = 0; n < textures.size(); n++) {
      bool is_selected = (currentTexture == n);
      if (ImGui::Selectable(textures[n].c_str(), is_selected))
        currentTexture = n;
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  // Button for deleting object
  if (ImGui::Button("Delete Object")) {
    // Handle delete object action
  }

  ImGui::End();
}

UI::OptionalObjectModificationType UI::getCurrentObjectModification() {
  if (CurrentObjectName == "") {
    return {};
  }
  return ObjectModification;
}
void UI::setCurrentObjectModification(
    UI::ObjectModificationType NewObjectModification) {
  ObjectModification = NewObjectModification;
}
