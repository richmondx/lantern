/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/area_light.h"

#include "scene/scene.h"

#include "math/sampling.h"


namespace Lantern {

float3 AreaLight::SampleLi(UniformSampler *sampler, Scene *scene, float3a &surfacePos, float3a &surfaceNormal, float3a *wi, float *pdf) const {
	// Generate a random point on the great circle of the bounding sphere that is oriented towards the origin
	float x, y;
	UniformSampleDisc(sampler, m_boundingSphere.w, &x, &y);
	
	// Calculate the paramaters of the great circle
	float3a circleDirection = (float3a)m_boundingSphere.xyz() - surfacePos;
	float distance = length(circleDirection);
	float3a circleNormal = normalize(circleDirection);

	float3a point = RotateToWorld(x, y, distance - m_boundingSphere.w, circleNormal);
	float3a direction = normalize(point);
	*wi = direction;

	// Check that the point on the circle is above the horizon
	if (dot(direction, surfaceNormal) <= 0.0f) {
		*pdf = 0.0f;
		return float3(0.0f);
	}

	// Make sure there's nothing occluding us
	RTCRay ray;
	ray.org = surfacePos;
	ray.dir = direction;
	ray.tnear = 0.001f;
	ray.tfar = embree::inf;
	ray.geomID = RTC_INVALID_GEOMETRY_ID;
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.instID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = 0xFFFFFFFF;
	ray.time = 0.0f;

	scene->Intersect(ray);
	if (ray.geomID != m_geomId) {
		*pdf = 0.0f;
		return float3(0.0f);
	}

	// Calculate the pdf
	float3a intersectionPoint = ray.org + ray.dir * ray.tfar;
	float distanceSquared = sqr_length(intersectionPoint - surfacePos);
	*pdf = distanceSquared / (std::abs(dot(normalize(ray.Ng), -direction)) * m_area);

	// Return the full radiance value
	// The value will be attenuated by the BRDF
	return m_radiance;
}

float AreaLight::PdfLi(Scene *scene, float3a &surfacePos, float3a &surfaceNormal, float3a &wi) const {
	// Check that wi is above the horizon
	if (dot(wi, surfaceNormal) <= 0.0f) {
		return 0.0f;
	}

	// Make sure there's nothing occluding us
	RTCRay ray;
	ray.org = surfacePos;
	ray.dir = wi;
	ray.tnear = 0.001f;
	ray.tfar = embree::inf;
	ray.geomID = RTC_INVALID_GEOMETRY_ID;
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.instID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = 0xFFFFFFFF;
	ray.time = 0.0f;

	scene->Intersect(ray);
	if (ray.geomID != m_geomId) {
		return 0.0f;
	}

	// Calculate the pdf
	float3a intersectionPoint = ray.org + ray.dir * ray.tfar;
	float distanceSquared = sqr_length(intersectionPoint - surfacePos);
	
	return distanceSquared / (std::abs(dot(normalize(ray.Ng), -wi)) * m_area);
}

} // End of namespace Lantern
