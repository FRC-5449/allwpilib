#include "glass/other/Field2D.h"
#include "glass/imgui_plot.h"
#include <imgui.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui_internal.h>
using namespace ImGui;
namespace imgplot {

void Plot(const char* label, const PlotConfig& conf) {
    // PlotStatus status = PlotStatus::nothing;

    // ImGuiWindow* window = GetCurrentWindow();
    // if (window->SkipItems)
    //     return status;

    const float* const* ys_list = conf.values.ys_list;
    int ys_count = conf.values.ys_count;
    const ImU32* colors = conf.values.colors;
    auto drawList = ImGui::GetWindowDrawList();
    glass::FieldFrameData inner_bb=conf.ffd;
    if (conf.values.count > 0) {
        int res_w;
        if (conf.skip_small_lines)
            res_w = ImMin((int)conf.frame_size.x, conf.values.count);
        else
            res_w = conf.values.count;
        res_w -= 1;
        int item_count = conf.values.count - 1;

        float x_min = conf.values.offset;
        float x_max = conf.values.offset + conf.values.count - 1;
        if (conf.values.xs) {
            x_min = conf.values.xs[size_t(x_min)];
            x_max = conf.values.xs[size_t(x_max)];
        }

        if (conf.grid_x.show) {
            float y0 = inner_bb.min.y;
            float y1 = inner_bb.max.y;
            switch (conf.scale.type) {
            case PlotConfig::Scale::Linear: {
                float cnt = conf.frame_size.x / (inner_bb.scale/5.0);
                for (int i = 0; i < cnt; ++i) {
                    float x0 = inner_bb.min.x + (i+1)*(inner_bb.scale/5.0);
                    drawList->AddLine(
                        ImVec2(x0, y0),
                        ImVec2(x0, y1),
                        IM_COL32(200, 200, 200, (i % conf.grid_x.subticks) ? 128 : 255));
                    char numstr[21];
                    // each maker is 0.2 meters
                    sprintf(numstr, "%.1f", (i+1)*0.2);
                    drawList->AddText(NULL, 0.0f, ImVec2(x0, y1), IM_COL32(255, 0, 0, 255), numstr, NULL, 0.0f, NULL);
                    // RenderTextClipped(ImVec2(x0, y0),  ImVec2(x0+200, y0+100), numstr, NULL, NULL, ImVec2(0.0f,0.0f));
                }
                break;
            }
            case PlotConfig::Scale::Log10: {
                float start = 1.f;
                while (start < x_max) {
                    for (int i = 1; i < 10; ++i) {
                        float x = start * i;
                        if (x < x_min) continue;
                        if (x > x_max) break;
                        float t = log10(x / x_min) / log10(x_max / x_min);
                        int x0 = ImLerp(inner_bb.min.x, inner_bb.max.x, t);
                        drawList->AddLine(
                            ImVec2(x0, y0),
                            ImVec2(x0, y1),
                            IM_COL32(200, 200, 200, (i > 1) ? 128 : 255));
                    }
                    start *= 10.f;
                }
                break;
            }
            }
        }
        if (conf.grid_y.show) {
            float x0 = inner_bb.min.x;
            float x1 = inner_bb.max.x;
            float cnt = conf.frame_size.y / (inner_bb.scale/5.0);
            for (int i = 0; i < cnt; ++i) {
                float y0 = inner_bb.max.y - (i+1)*(inner_bb.scale/5.0);
                drawList->AddLine(
                    ImVec2(x0, y0),
                    ImVec2(x1, y0),
                    IM_COL32(0, 0, 0, (i % conf.grid_y.subticks) ? 16 : 64));
                char numstr[21]; // enough to hold all numbers up to 64-bits
                sprintf(numstr, "%.1f", (i+1)*0.2);
                drawList->AddText(NULL, 0.0f, ImVec2(x0, y0), IM_COL32(255, 0, 0, 255), numstr, NULL, 0.0f, NULL);
                // RenderTextClipped(ImVec2(x0, y0),  ImVec2(x0+200, y0+100), numstr, NULL, NULL, ImVec2(0.0f,0.0f));
            }
        }
        
        ImU32 col_base = GetColorU32(ImGuiCol_PlotLines);
        for (int i = 0; i < ys_count; ++i) {
            if (colors) {
                if (colors[i]) col_base = colors[i];
                else col_base = GetColorU32(ImGuiCol_PlotLines);
            }

            float x0 = conf.values.xs[0];
            float y0 = ys_list[i][0];
            int count = 0;
            while (((ImAbs(x0)-0.03) < 0) && ((ImAbs(y0)-0.03) < 0)){
                count++;
                if(count < item_count){ 
                    x0 = conf.values.xs[count];
                    y0 = ys_list[i][count];
                }else break;
            }
            count++;
            // Point in the normalized space of our target rectangle
            ImVec2 pos0 = ImVec2(x0, y0);
            int pos1Flag = 0;
            ImVec2 pos1 = ImVec2(x0, y0);
            int n = count;
            int nt = n;
            for ( ;n < item_count; n++)
            {
                float x1 = conf.values.xs[n];
                float y1 = ys_list[i][n];
                if ( ((ImAbs(x1)-0.03) < 0) && ((ImAbs(y1)-0.03) < 0) ){
                    continue;
                }
                pos1 = ImVec2(x1,y1);
                pos1Flag = 1;
                drawList->AddLine(
                    pos0,
                    pos1,
                    col_base,
                    conf.line_thickness);
                char numstr[21]; // enough to hold all numbers up to 64-bits
                sprintf(numstr, "%d[%.1f:%.1f]", nt- 1, ((pos0.x - inner_bb.min.x)/inner_bb.scale), ((inner_bb.max.y - pos0.y)/ inner_bb.scale) );
                //drawList->AddText(NULL, 0.0f, pos0, IM_COL32(255, 0, 0, 255), numstr, NULL, 0.0f, NULL);
                drawList->AddCircleFilled(pos0, 5, IM_COL32(255, 0, 0, 255));
                pos0 = pos1;
                nt++;
            }
            if(pos1Flag ==1){
                char numstr[21]; // enough to hold all numbers up to 64-bits
                sprintf(numstr, "%d[%.1f:%.1f]", nt, ((pos1.x - inner_bb.min.x)/inner_bb.scale), ((inner_bb.max.y - pos1.y)/ inner_bb.scale) );
                // sprintf(numstr, "%d[%.1f:%.1f]", nt,pos1.x,pos1.y);
                //drawList->AddText(NULL, 0.0f, pos1, IM_COL32(255, 0, 0, 255), numstr, NULL, 0.0f, NULL);
                drawList->AddRectFilled(pos1, pos1 + ImVec2(8,8), IM_COL32(255, 0, 0, 255));
            }

        }

     }

}
}
