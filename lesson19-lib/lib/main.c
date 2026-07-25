#include <ncurses.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

#define M_PI 3.14
#define WIDTH 70
#define HEIGHT 35

// 心形参数方程
double heart_x(double t, double scale) {
    return scale * (16 * sin(t) * sin(t) * sin(t));
}

double heart_y(double t, double scale) {
    return scale * (13 * cos(t) - 5 * cos(2*t) - 2 * cos(3*t) - cos(4*t));
}

int main() {
    int ch, row, col;
    double t;
    int x, y;
    int points[1000][2];
    int num_points = 0;
    double scale = 0.8;
    int center_x, center_y;
    int frame = 0;
    int beat = 0;

    // 初始化 ncurses
    initscr();
    raw();
    noecho();
    curs_set(0);
    timeout(0);

    // 获取屏幕尺寸
    getmaxyx(stdscr, row, col);
    center_x = col / 2;
    center_y = row / 2;

    // 预计算心形所有点
    for (t = 0; t < 2 * M_PI; t += 0.01) {
        double px = heart_x(t, scale);
        double py = heart_y(t, scale);
        if (num_points < 1000) {
            points[num_points][0] = (int)(center_x + px);
            points[num_points][1] = (int)(center_y - py);
            num_points++;
        }
    }

    // 主循环
    while (1) {
        clear();

        // 动态跳动效果：缩放系数随帧变化
        double beat_scale = 1.0 + 0.08 * sin(frame * 0.08);
        int offset_x = (int)(10 * sin(frame * 0.06));
        int offset_y = (int)(5 * sin(frame * 0.1));

        // 绘制心形
        for (int i = 0; i < num_points; i++) {
            int px = (int)((points[i][0] - center_x) * beat_scale + center_x + offset_x);
            int py = (int)((points[i][1] - center_y) * beat_scale + center_y + offset_y);

            // 检查是否在屏幕范围内
            if (px >= 0 && px < col && py >= 0 && py < row) {
                mvprintw(py, px, "*");
            }
        }

        // 显示提示信息
        mvprintw(row - 2, 2, "按 'q' 退出");
        mvprintw(row - 1, 2, "❤️ 心形跳动");

        refresh();
        sleep(30000);
        frame++;

        // 检查退出
        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
    }

    // 清理 ncurses
    endwin();
    return 0;
}
