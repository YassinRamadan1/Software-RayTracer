#include "bvh.h"
#include "material.h"
#include "camera.h"

void bouncingSpheres()
{
    HittableList world;

    auto checker = std::make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));
    world.add(std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = RandomDouble();
            vec3 center(a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    Color albedo = Vec3::random() * Vec3::random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    Color albedo = Vec3::random(0.5, 1);
                    double fuzz = RandomDouble(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(vec3(4, 1, 0), 1.0, material3));

    Camera cam;

    cam.aspect_ratio_ = 16.0 / 9.0;
    cam.image_width_ = 400;
    cam.samples_per_pixel_ = 100;
    cam.max_depth_ = 50;
    cam.background_color_ = Color(0.70, 0.80, 1.00);

    cam.vertical_fov_ = 20;
    cam.look_from_ = vec3(13, 2, 3);
    cam.look_at_ = vec3(0, 0, 0);
    cam.world_up_ = vec3(0, 1, 0);

    cam.defocus_angle_ = 0.6;
    cam.focus_distance_ = 10.0;
    cam.init();
    cam.image_ = new TGAImage(cam.image_width_, cam.image_height_, TGAImage::RGB);

    BVHNode node(world);
    //world = HittableList(std::make_shared<BVHNode>(world));
    cam.render(node);
}

void checkeredSpheres()
{
    HittableList world;

    auto checker = std::make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));

    world.add(std::make_shared<Sphere>(vec3(0, -10, 0), 10, std::make_shared<Lambertian>(checker)));
    world.add(std::make_shared<Sphere>(vec3(0, 10, 0), 10, std::make_shared<Lambertian>(checker)));

    Camera cam;

    cam.aspect_ratio_ = 16.0 / 9.0;
    cam.image_width_ = 400;
    cam.samples_per_pixel_ = 100;
    cam.max_depth_ = 50;
    cam.background_color_ = Color(0.70, 0.80, 1.00);

    cam.vertical_fov_ = 20;
    cam.look_from_ = vec3(13, 2, 3);
    cam.look_at_ = vec3(0, 0, 0);
    cam.world_up_ = vec3(0, 1, 0);

    cam.defocus_angle_ = 0.0;
    cam.focus_distance_ = 10.0;
    cam.init();
    cam.image_ = new TGAImage(cam.image_width_, cam.image_height_, TGAImage::RGB);

    cam.render(world);
}

void earth()
{
    HittableList world;

    auto earth_texture = std::make_shared<ImageTexture>("C:/dev/RayTracing learn/RayTracing learn/res/earthmap.tga");
    auto earth_surface = std::make_shared<Lambertian>(earth_texture);
    world.add(std::make_shared<Sphere>(vec3(0, 0, 0), 2, earth_surface));

    Camera cam;

    cam.aspect_ratio_ = 16.0 / 9.0;
    cam.image_width_ = 400;
    cam.samples_per_pixel_ = 100;
    cam.max_depth_ = 50;
    cam.background_color_ = Color(0.70, 0.80, 1.00);

    cam.vertical_fov_ = 20;
    cam.look_from_ = vec3(0, 0, 12);
    cam.look_at_ = vec3(0, 0, 0);
    cam.world_up_ = vec3(0, 1, 0);

    cam.defocus_angle_ = 0.0;
    cam.focus_distance_ = 10.0;
    cam.init();
    cam.image_ = new TGAImage(cam.image_width_, cam.image_height_, TGAImage::RGB);

    cam.render(world);
}

void noiseSpheres()
{
    HittableList world;
    auto value_noise = std::make_shared<ValueNoise>(256);
    auto perlin_noise = std::make_shared<PerlinNoise>(256);
    auto noise_texture = std::make_shared<NoiseTexture>(perlin_noise, 4);
    world.add(std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(noise_texture)));
    world.add(std::make_shared<Sphere>(vec3(0, 2, 0), 2, std::make_shared<Lambertian>(noise_texture)));

    Camera cam;

    cam.aspect_ratio_ = 16.0 / 9.0;
    cam.image_width_ = 400;
    cam.samples_per_pixel_ = 50;
    cam.max_depth_ = 50;
    cam.background_color_ = Color(0.70, 0.80, 1.00);

    cam.vertical_fov_ = 20;
    cam.look_from_ = vec3(13, 2, 15);
    cam.look_at_ = vec3(0, 0, 0);
    cam.world_up_ = vec3(0, 1, 0);

    cam.defocus_angle_ = 0.0;
    cam.focus_distance_ = 10.0;
    cam.init();
    cam.image_ = new TGAImage(cam.image_width_, cam.image_height_, TGAImage::RGB);

    cam.render(world);
}

void quads()
{
    HittableList world;

    // Materials
    auto left_red = std::make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
    auto back_green = std::make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
    auto right_blue = std::make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
    auto lower_teal = std::make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    // Quads
    world.add(std::make_shared<Quad>(vec3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    world.add(std::make_shared<Quad>(vec3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(std::make_shared<Quad>(vec3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(std::make_shared<Quad>(vec3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(std::make_shared<Quad>(vec3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

    Camera cam;

    cam.aspect_ratio_ = 1.0f;
    cam.image_width_ = 400;
    cam.samples_per_pixel_ = 100;
    cam.max_depth_ = 50;
    cam.background_color_ = Color(0.70, 0.80, 1.00);

    cam.vertical_fov_ = 80;
    cam.look_from_ = vec3(0, 0, 9);
    cam.look_at_ = vec3(0, 0, 0);
    cam.world_up_ = vec3(0, 1, 0);

    cam.defocus_angle_ = 0.0;
    cam.focus_distance_ = 10.0;
    cam.init();
    cam.image_ = new TGAImage(cam.image_width_, cam.image_height_, TGAImage::RGB);

    cam.render(world);
}

void simpleLight()
{
    HittableList world;

    auto perlin_noise = std::make_shared<PerlinNoise>(256);
    auto perlin_texture = std::make_shared<NoiseTexture>(perlin_noise, 4);
    world.add(std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(perlin_texture)));
    world.add(std::make_shared<Sphere>(vec3(0, 2, 0), 2, std::make_shared<Lambertian>(perlin_texture)));

    auto difflight = std::make_shared<DiffuseLight>(Color(4, 4, 4));
    world.add(std::make_shared<Sphere>(vec3(0, 7, 0), 2, difflight));
    world.add(std::make_shared<Quad>(vec3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), difflight));

    Camera cam;

    cam.aspect_ratio_ = 16.0 / 9.0;
    cam.image_width_ = 1200;
    cam.samples_per_pixel_ = 700;
    cam.max_depth_ = 70;
    cam.background_color_ = Color(0, 0, 0);

    cam.vertical_fov_ = 20;
    cam.look_from_ = vec3(26, 3, 6);
    cam.look_at_ = vec3(0, 2, 0);
    cam.world_up_ = vec3(0, 1, 0);

    cam.defocus_angle_ = 0.0;
    cam.focus_distance_ = 10.0;
    cam.init();
    cam.image_ = new TGAImage(cam.image_width_, cam.image_height_, TGAImage::RGB);

    cam.render(world);
}

void cornellBox()
{
    HittableList world;

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    world.add(std::make_shared<Quad>(vec3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(std::make_shared<Quad>(vec3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(std::make_shared<Quad>(vec3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    world.add(std::make_shared<Quad>(vec3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(std::make_shared<Quad>(vec3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(std::make_shared<Quad>(vec3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    std::shared_ptr<Hittable> box1 = Box(vec3(0, 0, 0), vec3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    std::shared_ptr<Hittable> box2 = Box(vec3(0, 0, 0), vec3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, vec3(130, 0, 65));
    world.add(box2);

    Camera cam;

    cam.aspect_ratio_ = 1.0;
    cam.image_width_ = 1000;
    cam.samples_per_pixel_ = 800;
    cam.max_depth_ = 80;
    cam.background_color_ = Color(0, 0, 0);

    cam.vertical_fov_ = 40;
    cam.look_from_ = vec3(278, 278, -800);
    cam.look_at_ = vec3(278, 278, 0);
    cam.world_up_ = vec3(0, 1, 0);

    cam.defocus_angle_ = 0.0;
    cam.focus_distance_ = 10.0;
    cam.init();
    cam.image_ = new TGAImage(cam.image_width_, cam.image_height_, TGAImage::RGB);

    cam.render(world);
}

int main() {

    switch (7)
    {
    case 1:
        bouncingSpheres();
        break;
    case 2:
        checkeredSpheres();
        break;
    case 3:
        earth();
        break;
    case 4:
        noiseSpheres();
        break;
    case 5:
        quads();
        break;
    case 6:
        simpleLight();
        break;
    case 7:
        cornellBox();
        break;
    }
	return 0;
}