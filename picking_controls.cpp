#include "picking_controls.h"
namespace giv {
namespace io {

givr::vec2f pixelToNDC_2D(givr::vec2f const &pixel, int viewX, int viewY,
                          int viewWidth, int viewHeight) {
  return {(2.f / viewWidth) * (pixel.x - viewX) - 1.f,
          1.f - (2.f / viewHeight) * (pixel.y - viewY)};
}

givr::vec2f ndcToPixel_2D(givr::vec2f const &ndc, int viewX, int viewY,
                          int viewWidth, int viewHeight) {
  return {(ndc.x + 1.f) * (viewWidth / 2.f) + viewX,
          (1 - ndc.y) * (viewHeight / 2.f) + viewY};
}

givr::vec3f ndcToDeptPixel(givr::vec3f const &ndc, int viewX, int viewY,
                           int viewWidth, int viewHeight) {
  return {ndcToPixel_2D(givr::vec2f{ndc}, viewX, viewY, viewWidth, viewHeight),
          ndc.z};
}

givr::vec4f toHomogeneousCoordinate(givr::vec3f const &ndc, float w) {
  // w = 0 : vector
  // w = 1 : point
  return {ndc, w};
}

givr::vec4f perspectiveDivide(givr::vec4f homogeneousCoordinate) {
  homogeneousCoordinate /= homogeneousCoordinate.w;
  return homogeneousCoordinate;
}

givr::vec3f pixelToNDC(int pixelX, int pixelY, float ndcDepth,
                       int viewportWidth, int viewportHeight) {
  auto ndc2D = pixelToNDC_2D(givr::vec2f(pixelX, pixelY), //
                             0.f, 0.f,                    // viewport start x, y
                             viewportWidth, viewportHeight);
  return {ndc2D, ndcDepth};
}

givr::vec3f pixelToWorld3D(int pixelX, int pixelY, int viewportWidth,
                           int viewportHeight, givr::mat4f const &invMVP,
                           float ndcDepth) {
  using namespace givr;
  auto ndc =
      pixelToNDC(pixelX, pixelY, ndcDepth, viewportWidth, viewportHeight);
  auto hp = toHomogeneousCoordinate(ndc, 1.f); // points

  hp = invMVP * hp;
  hp = perspectiveDivide(hp);

  return vec3f(hp);
}

givr::vec3f world3DToNDC(givr::vec3f const &point, givr::mat4f const &MVP) {
  using namespace givr;
  auto hp = toHomogeneousCoordinate(point, 1.f);
  hp = MVP * hp;
  hp = perspectiveDivide(hp);
  return vec3f(hp);
}

givr::vec3f world3DToDepthPixel(givr::vec3f const &point,
                                givr::mat4f const &MVP, int viewX, int viewY,
                                int viewWidth, int viewHeight) {
  auto ndc = world3DToNDC(point, MVP);
  return ndcToDeptPixel(ndc, viewX, viewY, viewWidth, viewHeight);
}

} // namespace io
} // namespace giv
