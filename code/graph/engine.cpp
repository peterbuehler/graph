#include "private.h"

// Makro, um Ressource freizugeben
#define SafeRelease(p) { \
    if ((p) != NULL) { \
        (p)->Release(); \
        (p) = NULL; \
    } \
} \

// Vorwärtsdeklarationen
static void engine_paint_title(engine_t* engine);
static void engine_paint_grid(engine_t* engine);
static void engine_paint_border(engine_t* engine);
static void engine_paint_minorgrid(engine_t* engine);
static void engine_paint_row(engine_t* engine, row_t* row, ID2D1SolidColorBrush* brush);
static void engine_paint_data(engine_t* engine);
static void engine_paint_ticklabels(engine_t* engine);
static void engine_drawline(ID2D1HwndRenderTarget* target, point_t p1, point_t p2, ID2D1SolidColorBrush* brush, double linewidth);
static dim_t engine_size_textbox(string_t text, int fontsize);

engine_t* engine_create(graphics_t *graphics) {

	engine_t* engine = (engine_t*)malloc(sizeof(engine_t));
	*engine = {};

    engine->graphics = graphics;

    ID2D1Factory* pD2DFactory = NULL;
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
    engine->factory = pD2DFactory;
	return engine;
}


void engine_create_target(engine_t* engine, HWND hwnd) {

    RECT rc;
    GetClientRect(hwnd, &rc);

    ID2D1HwndRenderTarget* pRT = NULL;
    HRESULT hr = engine->factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(
            hwnd,
            D2D1::SizeU(
                rc.right - rc.left,
                rc.bottom - rc.top)
        ),
        &pRT
    );

    engine->target = pRT;
}

void engine_create_brush(engine_t* engine) {

    ID2D1SolidColorBrush* pBlackBrush = NULL;
    engine->target->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &pBlackBrush
    );
    engine->blackBrush = pBlackBrush;

    ID2D1SolidColorBrush* pRedBrush = NULL;
    engine->target->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::IndianRed),
        &pRedBrush
    );
    engine->redBrush = pRedBrush;

    ID2D1SolidColorBrush* pGrayBrush = NULL;
    engine->target->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::DarkGray),
        &pGrayBrush
    );
    engine->grayBrush = pGrayBrush;

    int colors[] = {    D2D1::ColorF::DarkOrange, D2D1::ColorF::DarkBlue, D2D1::ColorF::DarkRed,
                        D2D1::ColorF::DarkGreen, D2D1::ColorF::DarkCyan, D2D1::ColorF::DarkMagenta };

    for (int i = 0; i < BRUSH_COUNT_ROWS; i++) {
        ID2D1SolidColorBrush* pBrush = NULL;
        engine->target->CreateSolidColorBrush(
            D2D1::ColorF(colors[i]),
            &pBrush
        );
        engine->brushes[i] = pBrush;
    }

}

void engine_create_text(engine_t* engine) {

    static const WCHAR msc_fontName[] = L"Arial";

    IDWriteFactory* pDWriteFactory;

    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(pDWriteFactory),
        reinterpret_cast<IUnknown**>(&pDWriteFactory)
    );

    engine->writeFactory = pDWriteFactory;

    IDWriteTextFormat* m_pTextFormat1;
    engine->writeFactory->CreateTextFormat(
        msc_fontName,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        FONT_SIZE_TICKLABELS,     // fontsize
        L"",    //locale
        &m_pTextFormat1
    );
    m_pTextFormat1->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_pTextFormat1->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    engine->textFormatXTickLabels = m_pTextFormat1;

    IDWriteTextFormat* m_pTextFormat2;
    engine->writeFactory->CreateTextFormat(
        msc_fontName,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        FONT_SIZE_TICKLABELS,     // fontsize
        L"",    //locale
        &m_pTextFormat2
    );
    m_pTextFormat2->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    m_pTextFormat2->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    engine->textFormatYTickLabels = m_pTextFormat2;

    IDWriteTextFormat* m_pTitleFormat;
    engine->writeFactory->CreateTextFormat(
        msc_fontName,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        FONT_SIZE_TITLE,     // fontsize
        L"",    //locale
        &m_pTitleFormat
    );
    m_pTitleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_pTitleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    engine->textFormatTitle = m_pTitleFormat;
}

void engine_window_resized(engine_t* engine, int width, int height) {

    graphics_set_size(engine->graphics, { (double)width, (double)height });
    engine->target->Resize(D2D1::SizeU(width, height));

}

void engine_paint(engine_t* engine) {

    //printf("-> paint()");

    ID2D1HwndRenderTarget* t = engine->target;

    t->BeginDraw();
    t->Clear(D2D1::ColorF(D2D1::ColorF::White));

    engine_paint_grid(engine);
    engine_paint_minorgrid(engine);
    engine_paint_border(engine);
    engine_paint_data(engine);
    engine_paint_ticklabels(engine);
    engine_paint_title(engine);


    t->EndDraw();
}

static void engine_paint_grid(engine_t* engine) {

    ID2D1HwndRenderTarget* t = engine->target;
    graphics_t* g = engine->graphics;

    // xgrid
    for (int i= 0; i<g->xaxis->ticks->length; i++) {
        double x = g->xaxis->ticks->values[i];
        point_t p1 = graph_to_view(g, { x, g->yaxis->limit.min });
        point_t p2 = graph_to_view(g, { x, g->yaxis->limit.max });
        engine_drawline(t, p1, p2, engine->grayBrush, 0.5);
    }

    // ygrid
    for (int i = 0; i < g->yaxis->ticks->length; i++) {
        double y = g->yaxis->ticks->values[i];
        point_t p1 = graph_to_view(g, { g->xaxis->limit.min, y });
        point_t p2 = graph_to_view(g, { g->xaxis->limit.max, y });
        engine_drawline(t, p1, p2, engine->grayBrush, 0.5);
    }

}

static void engine_paint_minorgrid(engine_t* engine) {

    ID2D1HwndRenderTarget* t = engine->target;
    graphics_t* g = engine->graphics;

    // xgrid
    if (g->xaxis->scale == SCALE_LOG) {
        for (int i = 0; i < g->xaxis->minorticks->length; i++) {
            double x = g->xaxis->minorticks->values[i];
            point_t p1 = graph_to_view(g, { x, g->yaxis->limit.min });
            point_t p2 = graph_to_view(g, { x, g->yaxis->limit.max });
            engine_drawline(t, p1, p2, engine->grayBrush, 0.25);
        }
    }

    // ygrid
    if (g->yaxis->scale == SCALE_LOG) {
        for (int i = 0; i < g->yaxis->minorticks->length; i++) {
            double y = g->yaxis->minorticks->values[i];
            point_t p1 = graph_to_view(g, { g->xaxis->limit.min, y });
            point_t p2 = graph_to_view(g, { g->xaxis->limit.max, y });
            engine_drawline(t, p1, p2, engine->grayBrush, 0.25);
        }
    }

}

static void engine_paint_border(engine_t* engine) {

    ID2D1HwndRenderTarget* t = engine->target;
    graphics_t* g = engine->graphics;

    point_t p1 = graph_to_view(g, { g->xaxis->limit.min, g->yaxis->limit.min });
    point_t p2 = graph_to_view(g, { g->xaxis->limit.max, g->yaxis->limit.max });

    D2D1_RECT_F rect = D2D1::RectF((float) p1.x, (float) p1.y, (float) p2.x, (float) p2.y);

    t->DrawRectangle(&rect, engine->grayBrush);

}

static void engine_paint_data(engine_t* engine) {

    graphics_t* g = engine->graphics;

    for (int i = 0; i < g->data->length; i++) {
        int iBrush = i % BRUSH_COUNT_ROWS;
        engine_paint_row(engine, g->data->rows[i], engine->brushes[iBrush]);
    }
}

static void engine_paint_row(engine_t* engine, row_t *row, ID2D1SolidColorBrush* brush) {

    ID2D1HwndRenderTarget* t = engine->target;
    graphics_t* g = engine->graphics;

    //datarow_t* row = (datarow_t*) row1->priv;

    int length;
    point_t* points;
    row_get_points(row, &points, &length);

    for (int i = 0; i < length - 1; i++) {
        point_t p1 = graph_to_view(g, points[i]);
        point_t p2 = graph_to_view(g, points[i + 1]);
        engine_drawline(t, p1, p2, brush, 1.0);
    }
}

static void engine_paint_ticklabels(engine_t* engine) {

    ID2D1HwndRenderTarget* t = engine->target;
    graphics_t* g = engine->graphics;

    // xticklabels
    for (int i = 0; i < g->xaxis->ticklabels->length; i++) {

        double x = g->xaxis->ticklabels->values[i];
        string_t s = g->xaxis->ticklabels->texts[i];
        point_t p = graph_to_view(g, { x, g->yaxis->limit.min });

        dim_t size = engine_size_textbox(s, FONT_SIZE_TICKLABELS);
        double yoffs = size.height / 2;
        double x1 = p.x - size.width / 2;
        double y1 = p.y + yoffs;
        double x2 = p.x + size.width / 2;
        double y2 = p.y + size.height + yoffs;
        D2D1_RECT_F rect = D2D1::RectF((float) x1, (float) y1, (float) x2, (float) y2);

        int len = (int) wcslen(s);
        t->DrawText( s, len, engine->textFormatXTickLabels, rect, engine->blackBrush);
        // t->DrawRectangle(rect, engine->blackBrush);
    }

    // yticklabels
    for (int i = 0; i < g->yaxis->ticklabels->length; i++) {

        double y = g->yaxis->ticklabels->values[i];
        string_t s = g->yaxis->ticklabels->texts[i];
        point_t p = graph_to_view(g, { g->xaxis->limit.min, y });

        dim_t size = engine_size_textbox(s, FONT_SIZE_TICKLABELS);
        double xoffs = size.height / 2;
        double yoffs = size.height / 2;
        double x1 = p.x - size.width - xoffs;
        double y1 = p.y - yoffs;
        double x2 = p.x - xoffs;
        double y2 = p.y + size.height - yoffs;
        D2D1_RECT_F rect = D2D1::RectF((float)x1, (float)y1, (float)x2, (float)y2);

        int len = (int) wcslen(s);
        t->DrawText(s, len, engine->textFormatYTickLabels, rect, engine->blackBrush);
        // t->DrawRectangle(rect, engine->blackBrush);
    }
}

static void engine_paint_title(engine_t* engine) {

    ID2D1HwndRenderTarget* t = engine->target;
    graphics_t* g = engine->graphics;

    int length = (int)wcslen(g->title);
    if (length > 0) {

        dim_t size = engine_size_textbox(g->title, FONT_SIZE_TITLE);
        double x = g->size.width / 2;
        double y = g->border.top - size.height * 2;

        double x1 = x - size.width / 2;
        double y1 = y;
        double x2 = x + size.width / 2;
        double y2 = y + size.height;
        D2D1_RECT_F rect = D2D1::RectF((float)x1, (float)y1, (float)x2, (float)y2);
        t->DrawText(g->title, length, engine->textFormatTitle, rect, engine->blackBrush);

       // t->DrawRectangle(rect, engine->blackBrush);
    }

}

static dim_t engine_size_textbox(string_t text, int fontsize) {

    // simple guess
    int len = (int)wcslen(text);
    double width = len * fontsize * 0.7;
    double height = fontsize * 1.4;  // avoid cutting decent  
    return { width, height };
}

static void engine_drawline(ID2D1HwndRenderTarget* target, point_t p1, point_t p2, ID2D1SolidColorBrush* brush, double linewidth) {
    target->DrawLine(D2D1::Point2F((float) p1.x, (float) p1.y), D2D1::Point2F((float) p2.x, (float) p2.y), brush, (float) linewidth);
}


void engine_release(engine_t* engine) {
    SafeRelease(engine->grayBrush);
    SafeRelease(engine->redBrush);
    SafeRelease(engine->blackBrush);
    for (int i = 0; i < BRUSH_COUNT_ROWS; i++) {
        SafeRelease(engine->brushes[i]);
    }
    SafeRelease(engine->textFormatXTickLabels);
    SafeRelease(engine->textFormatYTickLabels);
    SafeRelease(engine->target);
    SafeRelease(engine->writeFactory);
    SafeRelease(engine->factory);
    
}
